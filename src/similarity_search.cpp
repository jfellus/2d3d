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
#include "common/config_reader.h"


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

void dump_config(const retin_config& rcf) {
	cout << "codebook dir: " << rcf.dict_dir << endl;
	cout << "index file: " << rcf.feature_file << endl;
	cout << "projectors file : " << rcf.proj_dir << "/" << rcf.proj_file << endl;
	cout << "power norm: " << rcf.feature_pn << "\n" << endl;
}


typedef struct {
	size_t id;
	float score;
} similarity_t;

int fn_compare_similarities (const void * a, const void * b) {
   return ( ((similarity_t*)b)->score - ((similarity_t*)a)->score )*1000;
}


/** Compute query signature */
float* compute_signature(const string& query, const retin_config& rcf, Codebook* codebook, float* proj, size_t nbProj) {
	cout << "computing descriptors for query ... ";
	float* query_descriptors;
	size_t nbDesc;
	if(!get_descriptors(query, rcf, query_descriptors, nbDesc)) {
		cerr << "Unable to extact/read descriptors from " << query << "\n";
		exit(-1);
	}
	cout << "ok\n";

	cout << "computing compact VLAT for query ... \n";
	float* vlat;
	float* compact_vlat;
	VlatBuilder* builder = create_vlat_builder(rcf, codebook);
	vlat = vlat_compute(query_descriptors, nbDesc, builder);
	vlat_normalize(vlat, builder->vlatDim, rcf.power_norm);
	compact_vlat = vlat_compact(vlat, proj, builder->vlatDim, nbProj);
	cout << "ok\n";
	return compact_vlat;
}



//! Program that computes the similarity of an image / a 3d model with the given indexed content
int main(int argc, char **argv) {
	try {
	// INIT
	string config_file = "config.properties";
	
	if(argc > 1) config_file = argv[1];

	cout << "config: " << config_file << endl;
	retin_config rcf = read_config_file(config_file);
	string query = rcf.query_file;

	if(argc > 2) query = argv[2];
	if(argc > 3) rcf.similarity_file = argv[3];
	
	dump_config(rcf);

	// GET QUERY
	stringstream ss(query);
	size_t docId;
	bool bQueryById = (ss >> docId);
	float* query_signature;

	// LOADING
	cout << "loading index ... ";
	Matrix index(rcf.feature_file);
	if(!index) { cout << "Error " << rcf.feature_file << endl; 	exit(-1); }
	cout << index.height << " features of size "<< index.width << endl;

	if(!bQueryById) {
		// Load codebook
		Codebook* codebook = new Codebook(rcf);
		cout << codebook->K << " features of size "<< codebook->descDim << endl;

		// Load projectors
		cout << "loading proj ... ";
		Matrix proj(rcf.proj_dir + "/" + rcf.proj_file);
		if(!proj) { cout << "Error " << rcf.proj_file << endl;	exit(-1);	}
		cout << proj.height << " features of size "<< proj.width << endl;

		if(proj.height!=index.width) {
			cout << "Error: Given index and projectors are incompatible" << endl;
			exit(-1);
		}

		preprocess_index(index, index.width, index.height, rcf);
		preprocess_projectors(proj, proj.width, rcf);

		query_signature = compute_signature(query, rcf, codebook, proj, proj.height);
	} else {
		preprocess_index(index, index.width, index.height, rcf);
		query_signature = index.get_row(docId);
	}
	

	// COMPUTE SIMILARITY
	cout << "compute similarities ... ";
	similarity_t* similarities = new similarity_t[index.height];
	for(size_t i = 0 ; i < index.height ; i++) {
		similarities[i].id = i;
		similarities[i].score = vector_ps_float(index.get_row(i), query_signature, index.width);
	}
	qsort(similarities, index.height, sizeof(similarity_t), fn_compare_similarities);


	ofstream file_sim(rcf.similarity_file.c_str(), ios_base::out);
	for(size_t i = 0 ; i < index.height ; i++) {
		int sim_percentage = (int)(0.5f+1.0f/(1.0f+exp(-rcf.similarity_score_sigmoid_factor*similarities[i].score))*100);
		file_sim << similarities[i].id << "=" << sim_percentage << "\n";
		//file_sim << similarities[i].id << "=" << similarities[i].score*100 << "\n";
	}
	file_sim.close();
	cout << "ok\n";

	return 0;
	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
