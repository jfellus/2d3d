/*
Copyright © CNRS 2012. 
Authors: David Picard, Philippe-Henri Gosselin, Romain Negrel, Hedi 
Tabia
Contact: picard@ensea.fr

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

 */
#include <iostream>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <libgen.h>
#include "common/config_reader.h"
#include <vector>
#include <string>
#include <algorithm>
#include <microhttpd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>


#include "retin/toolbox/core/fvec_writer.h"
#include "retin/toolbox/core/fvec_reader.h"
#include "retin/feature/depthimages/DepthImages.h"
#include "common/2D3D.h"
#include "common/vlat.h"

using namespace std;
using retin::fvec_writer;
using vlat::StandardVlatBuilder;
using vlat::StandardPrePcaVlatBuilder;
using retin::DepthImages;

///////////
// TYPES //
///////////

#define PORT 10013
#define SIMILARITY_SCORE_SIGMOID 0.02

#define DBG(x) cout << x << endl;
#define DBGV(x) cout << #x << " = \"" << x << "\"" << endl;

#define CHECK_FILE(x) FILE* ffff = fopen(doc, "r"); if(ffff) fclose(ffff); else
#define CHECK_FILE_EMPTY(x) struct stat ___buf; stat(doc, &___buf); if(___buf.st_size == 0)


typedef struct {
	size_t id;
	float score;
} similarity_t;

int fn_compare_similarities (const void * a, const void * b) {
	return ( ((similarity_t*)b)->score - ((similarity_t*)a)->score )*1000;
}


/////////////
// GLOBALS //
/////////////

retin_config rcf = read_config_file("config.properties");

string list_path;
char* basedir;
string images_dir;

vector<string> doclist;
Matrix vlat_index;

sem_t sStop;


//////////
// INIT //
//////////

bool find_images_dir() {
	images_dir = basedir;

	FILE* f = fopen(list_path.c_str(), "r");
	if(!f) {
		fprintf(stderr, "ERROR : Cant' open images list : %s\n", list_path.c_str()); return false;
	}
	char* firstfile = 0; size_t n = 0;
	if(getline(&firstfile, &n, f)) {}
	firstfile[strlen(firstfile)-1] = 0;
	fclose(f);

	string s = images_dir + "/" + firstfile;
	f = fopen(s.c_str(), "r");
	if(f) {fclose(f); return true;}

	images_dir = images_dir + "/images";
	s = images_dir + "/" + firstfile;
	f = fopen(s.c_str(), "r");
	if(f) {fclose(f); return true;}
	return false;
}

void init_dirs(char** argv) {
	list_path = argv[2];			DBGV(list_path);
	basedir = dirname(argv[2]);		DBGV(basedir);
	if(!find_images_dir()) throw "Can't find images dir";
	DBGV(images_dir);
}

void init_doc_list() {
	ifstream f(list_path );
	for( string line; getline( f, line ); ) {
		doclist.push_back(line);
	}
}


////////////
// ANSWER //
////////////


void list_docs(FILE* fout) {
	for(int i=0; i<doclist.size(); i++) {
		DBG(doclist[i]);
		fprintf(fout, "%s\n", doclist[i].c_str());
	}
}

void similarity_search(float* query_signature, FILE* fout) {
	// COMPUTE SIMILARITY
	cout << "compute similarities ... ";
	similarity_t* similarities = new similarity_t[vlat_index.height];
	for(size_t i = 0 ; i < vlat_index.height ; i++) {
		similarities[i].id = i;
		similarities[i].score = vector_ps_float(vlat_index.get_row(i), query_signature, vlat_index.width);
	}
	qsort(similarities, vlat_index.height, sizeof(similarity_t), fn_compare_similarities);


	for(size_t i = 0 ; i < vlat_index.height ; i++) {
		//int sim_percentage = (int)(0.5f+1.0f/(1.0f+exp(-SIMILARITY_SCORE_SIGMOID*similarities[i].score))*100);
		int sim_percentage = (int)(100*similarities[i].score);
		fprintf(fout, "%s|%i\n", doclist[similarities[i].id].c_str(), sim_percentage);
		//file_sim << similarities[i].id << "=" << similarities[i].score*100 << "\n";
	}

	cout << "done\n";
}

void similarity_search(size_t docId, FILE* fout) {
	float* query_signature = vlat_index.get_row(docId);
	similarity_search(query_signature, fout);
}

int find_doc_id(const char* doc) {
	return find(doclist.begin(), doclist.end(), doc) - doclist.begin();
}

void similarity_search(const char* doc, FILE* fout) {
	similarity_search(vlat_index.get_row(find_doc_id(doc)), fout);
}


void compute_signature(const char* file, float* sigout) {
	static Codebook* codebook = NULL;
	static VlatBuilder* builder = NULL;

	if(!codebook) {	codebook = new Codebook(rcf); DBG(codebook->K << " features of size "<< codebook->descDim); }
	if(!builder) builder = create_vlat_builder(rcf, codebook);

	DBG("computing descriptors for query ... ");
	float* query_descriptors = NULL;
	size_t nbDesc;
	if(!get_descriptors(file, rcf, query_descriptors, nbDesc)) {
		DBG("Unable to extact/read descriptors from " << file); exit(-1);
	}
	DBG("ok");
	if(!query_descriptors) return;

	DBG("computing compact VLAT for query ... ");

	builder->process(query_descriptors, nbDesc);
	if(query_descriptors) { delete [] query_descriptors; query_descriptors = NULL; }

	builder->getVlat(sigout);
	vlat_normalize(sigout, builder->vlatDim, 0.5);
	DBG("ok");
}

void similarity_search_file(const char* doc, FILE* fout) {
	CHECK_FILE(doc) { fprintf(fout, "Can't open %s", doc); return;}
	CHECK_FILE_EMPTY(doc) {fprintf(fout, "Can't open %s", doc); return;}


	static float* query_signature = NULL;
	if(!query_signature) query_signature = new float[vlat_index.width];
	compute_signature(doc, query_signature);
	similarity_search(query_signature, fout);
}

void stop() {
	DBG("QUITTING...");
	sem_post(&sStop);
}

void query(const char* q, const char* v, FILE* fout) {
	if(!strcmp(q, "stop")) stop();
	else if(!strcmp(q, "similarity")) similarity_search(v, fout);
	else if(!strcmp(q, "simcam")) similarity_search_file("/dev/shm/cam.png",fout);
	else if(!strcmp(q, "list")) list_docs(fout);
	else if(!strcmp(q, "getdir")) {
		fprintf(fout, "%s", images_dir.c_str());
	}
}


//////////
// MAIN //
//////////


static int answer_to_connection (void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,size_t *upload_data_size, void **con_cls) {
	struct MHD_Response *response;
	int ret;

	FILE* ff = fopen("/dev/shm/simsearchserv.tmp", "w");
	struct stat sbuf;

	const char* q = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "q");
	if(q) {
		const char* v = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "v");
		printf("Q : %s(%s)\n", q, v);
		query(q, v, ff);
	} else printf("no q\n");

	fflush(ff);
	fclose(ff);
	int fd = open("/dev/shm/simsearchserv.tmp", O_RDONLY);
	fstat (fd, &sbuf);
	response = MHD_create_response_from_fd_at_offset (sbuf.st_size, fd, 0);
	//response =  MHD_create_response_from_buffer (strlen(s), (void *) s, MHD_RESPMEM_PERSISTENT);
	MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);

	return ret;
}

int main(int argc, char **argv) {
	try {
		sem_init(&sStop, 0, 0);

		// INIT
		if(argc < 3) throw "Not enough arguments";
		string index_file = argv[1];
		init_dirs(argv);
		init_doc_list();
		DBGV(index_file);

		// LOAD INDEX
		cout << "loading index ... ";
		vlat_index.load(index_file.c_str(), MATRIX_LOCAL);
		if(!vlat_index) { cout << "Error " << index_file << endl; exit(-1); }
		cout << vlat_index.height << " signatures of dimension "<< vlat_index.width << endl;
		preprocess_index(vlat_index, vlat_index.width, vlat_index.height, 0.5, true);

		// INIT SERVER
		struct MHD_Daemon *daemon;

		daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);
		if (NULL == daemon) return 1;

		char cwd[512]; if(getcwd(cwd, 512)){}
		string cmd = "chromium-browser --start-maximized --app=file:///";
		cmd += cwd;
		cmd += "/src/index.html";
		if(system(cmd.c_str()));

		sem_wait(&sStop);
		MHD_stop_daemon (daemon);

		return 0;
	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
