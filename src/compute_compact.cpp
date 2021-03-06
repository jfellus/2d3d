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
#include "common/multithread.h"
#include "common/tools.h"

using namespace std;

#include "retin/toolbox/core/SharedMatrix.h"
#include "retin/feature/vlat/StandardVlatBuilder.h"
#include "retin/feature/vlat/StandardPrePcaVlatBuilder.h"
#include "retin/feature/depthimages/DepthImages.h"

using vlat::StandardVlatBuilder;
using vlat::StandardPrePcaVlatBuilder;
using retin::DepthImages;
using namespace shared_matrices;


void dump_config(const retin_config& rcf) {
	cout << "vlat file: " << rcf.vlat_file << endl;
	cout << "power norm: " << rcf.power_norm << "\n" << endl;
}


typedef struct {  float *C; float *A; float* B; size_t w; size_t n; size_t h; } CpAB_t;
void CpAB_thread(CpAB_t* t, int i) {
	float *C = t->C; float *A = t->A; float* B = t->B;
	size_t w = t->w; size_t n = t->n;
	for(int j=0; j<w; j++) {
		for(int k=0; k<n; k++) {
			C[i*w + j] += A[i*n+ k]*B[k*w + j];
		}
	}
}
__multithread__(the_CpAB)(void* t, int i) {CpAB_thread((CpAB_t*)t, i);}
void matrix_CpAB_multithread(float *C, float *A, float* B, size_t w, size_t n, size_t h) {
	CpAB_t* t = new CpAB_t;
	t->C = C;
	t->A = A;
	t->B = B;
	t->w = w;
	t->n = n;
	t->h = h;
	DBGV(t->h);
	DBGV(t->n);
	DBGV(t->w);
	the_CpAB(t, h);
	delete t;
}









//! Program that compact a set of vlat signatures
int main(int argc, char **argv) {
	try {
		string config_file = "config.properties";
		if(argc > 1) config_file = argv[1];
		cout << "config: " << config_file << endl;
		retin_config rcf = read_config_file(config_file);
		dump_config(rcf);

		// load vlat
		cout << "loading vlat ... " ;
		Matrix vlat(rcf.vlat_file, MATRIX_LOCAL);
		if (!vlat) {cout << "no file or shared matrix " << rcf.vlat_file << endl;	exit(-1);}
		cout << vlat.height << " features of size "<< vlat.width << endl;

		cout << "power normalization ... ";
		vector_pow_float(vlat, rcf.power_norm, vlat.height*vlat.width);
		cout << "ok\n";
		cout << "l2 normalization ... ";
		for(size_t i = 0 ; i < vlat.height ; i++) {
			float n2 = vector_n2_float(vlat.get_row(i), vlat.width);
			vector_sdiv_float(vlat.get_row(i), n2, vlat.width);
		}
		cout << "ok\n";


		// read projectors
		cout << "loading proj ... ";
		Matrix proj(rcf.proj_dir + "/" + rcf.proj_file, MATRIX_LOCAL);
		if (!proj) {cout << "Error " << rcf.proj_file << endl;	exit(-1);}
		cout << proj.height << " features of size "<< proj.width << endl;

		if(rcf.transpose_proj) {
			if(vlat.width != proj.height) {
				cout << "Error: vlat dimension != projectors dimension" << endl;
				exit(-1);
			}

			if(rcf.proj_white) {
				cout << "whitening projectors ... ";
				Matrix weights(rcf.proj_dir+"/"+rcf.weight_file, MATRIX_LOCAL);
				if(!weights) {cout << "Error " << rcf.weight_file << endl;	exit(-1); }

				for(size_t i = 0 ; i < weights.width ; i++) {
					float s = 1.f/sqrtf(weights[i]);
					for(size_t j = 0; j<proj.height; j++)	proj[j*proj.width + i] *= s;
				}
				cout << "ok\n";
			}

			// project
			cout << "projecting on " << proj.width << " projectors ... ";
			Matrix compact(proj.width, vlat.height);
			compact.clear();
			matrix_CpAB_multithread(compact, proj, vlat, proj.width, proj.height, vlat.height);
			cout << "ok\n";

			// write results
			cout << "saving results in " << rcf.compact_file << " ... ";
			compact.save(rcf.compact_file);
			cout << "ok\n";

			if(rcf.release_vlat_after_compact) vlat.set_owner();

			cout << "finished" << endl;

			return 0;
		}




		if(vlat.width != proj.width) {
			cout << "Error: vlat dimension != projectors dimension" << endl;
			exit(-1);
		}

		if(rcf.proj_white) {
			cout << "whitening projectors ... ";
			Matrix weights(rcf.proj_dir+"/"+rcf.weight_file, MATRIX_LOCAL);
			if(!weights) {cout << "Error " << rcf.weight_file << endl;	exit(-1); }

			for(size_t j = 0 ; j < proj.height ; j++) {
				float s = 1.f/sqrtf(weights[j]);
				vector_smul_float(proj.get_row(j), s, proj.width);
			}
			cout << "ok\n";
		}

		// project
		cout << "projecting on " << proj.height << " projectors ... ";
		Matrix compact(proj.height, vlat.height);
		compact.clear();
		matrix_CpAtB_float(compact, proj, vlat, proj.height, proj.width, vlat.height);
		cout << "ok\n";

		// write results
		cout << "saving results in " << rcf.compact_file << " ... ";
		compact.save(rcf.compact_file);
		cout << "ok\n";

		if(rcf.release_vlat_after_compact) vlat.set_owner();

		cout << "finished" << endl;
		return 0;
	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
