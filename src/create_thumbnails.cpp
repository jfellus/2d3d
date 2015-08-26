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


#include <string>
#include <errno.h>
#include <iostream>
#include <ostream>
#include "common/tools.h"

#ifdef RETIN_ENABLE_OSMESA
#include "common/Renderer3D.h"
#include "retin/toolbox/codec3d/Codec3DOFF.h"
#include "retin/toolbox/codec3d/Codec3DPLY.h"
#endif

#include <dirent.h>
#include <vector>


using namespace std;
using namespace retin;



size_t WIDTH = 800, HEIGHT = 600;

size_t NBVIEWS = 10;
Vector3d* VIEWS;
float CAM_HEIGHT = 0.3f;

void _createViews() {
	VIEWS = new Vector3d[NBVIEWS];
	for(int i=0; i<NBVIEWS; i++) {
		VIEWS[i].x = 1.3*cos(i*2.0*3.141592/NBVIEWS);
		VIEWS[i].z = 1.3*sin(i*2.0*3.141592/NBVIEWS);
		VIEWS[i].y = 0.8;
	}
}

void process3dDocument(const string& in, const string& outdir) {
	cout << "3D\n";
#ifdef RETIN_ENABLE_OSMESA
	Decoder3D* dec = file_has_ext(in, ".ply") ?
			(Decoder3D*) new Decoder3DPLY() :
			(Decoder3D*) new Decoder3DOFF();
	Mesh mesh;
	dec->read_open(in);
	dec->read_header();
	dec->read_data(&mesh);
	delete dec;

	mesh.setScale(1);
	mesh.computeFaceNormalsOut();

	Renderer3D renderer(WIDTH, HEIGHT);

	float* img = new float[WIDTH*HEIGHT*3];
	char ii[11];
	for(size_t i=0; i<NBVIEWS; i++) {
		cout << "  view " << i << "\n";
		Vector3d cam = VIEWS[i];
		renderer.setCamera(cam);
		renderer.render(mesh, img);

		sprintf(ii, "%010lu", i);
		string outfile = outdir + "/" + basename(in.c_str()) + "_" + ii +".jpg";
		matrix_to_jpg(img, WIDTH, HEIGHT, outfile.c_str());
	}
	delete img;
#endif
}

void processDocument(const string& in, const string& outdir) {
//	if(file_has_ext(in,".off") || file_has_ext(in, ".ply")) {
		process3dDocument(in, outdir);
//	}
	//else cmd("convert %s  -resize %ux%u  %s/%s", in.c_str(), WIDTH, HEIGHT, outdir.c_str(), basename(in.c_str()));
}

void usage() {
	printf("RETIN THUMBNAILS GENERATOR\n");
	printf("*****************************\n");
	printf("  Usage : create_thumbnails [-s <width>x<height>] files_path thumbnails_out_path\n");
	printf("\n******************\n\n");
}


int main(int argc, char **argv) {
	try {
		if(argc<3) {
			usage();
			return 1;
		}
		_createViews();

		int arg = 1;
		if(string(argv[arg]) == "-s") {
			cout << "SIZE\n";
			stringstream s(string(argv[++arg]));
			char c;
			s >> WIDTH >> c >> HEIGHT;
			arg++;
		}
		cout << "WIDTH = " << WIDTH << ", HEIGHT = " << HEIGHT << "\n";

		string original_path = argv[arg++];
		string thumbnails_path = argv[arg++];

		vector<string> doc_list;
		list_directory(original_path,doc_list);

		for(int i=0; i<doc_list.size(); i++) {
			cout << "Create thumbnail(s) for " <<  doc_list[i] << " -> " << thumbnails_path << " ... ";
			processDocument(doc_list[i], thumbnails_path);
			cout << "ok\n";
		}

		return 0;
	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
