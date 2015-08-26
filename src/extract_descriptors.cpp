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

using namespace std;

#define THREAD_POOL_USE_POSIX_THREADS

#include <iostream>
#include "common/config_reader.h"
#include "retin/toolbox/quantization/ElbgQuantizer.h"
#include "retin/toolbox/core/SharedMatrix.h"
#include "common/2D3D.h"
#include "common/tools.h"
#include <sys/stat.h>

using quantization::ElbgQuantizer;
using retin::fvec_writer;
using namespace shared_matrices;


//! load descriptors for all doc in retin_config.doc_list
bool load_descriptors(vector<float>&, retin_config&);


void dump_config(const retin_config& rcf) {
	cout << "documents dir: "<< rcf.doc_dir << endl;
	cout << "documents list: "<< rcf.doc_list << endl;
	cout << "dict size: " << rcf.dict_size << endl;
	cout << "desc step: " << rcf.desc_step << endl;
	cout << "desc scales: ";
	for(size_t i = 0 ; i < rcf.desc_scales.size() ; i++)
		cout << rcf.desc_scales[i] << ",";
	cout << endl;
	cout << "number of views: " << rcf.nb_views << endl;
	cout << "dict_dir: " << rcf.dict_dir << endl;
	cout << "dict_file: " << rcf.dict_file << endl;
	cout << "mean1_file: " << rcf.mean1_file << endl;
	cout << "mean2_file: " << rcf.mean2_file << "\n" << endl;
}


//! program that computes a codebook on 2D images and 3D models
int main(int argc, char **argv) {
	try {
		string config_file = "config.properties";
		if(argc > 1) config_file = argv[1];
		cout << "config: " << config_file << endl;
		retin_config rcf = read_config_file(config_file);
		dump_config(rcf);

		vector<float> descriptors;
		if(! load_descriptors(descriptors, rcf)) {
			cerr << "Error generating descriptors" << endl;
			exit(-1);
		}

		size_t descriptorDim = 128;
		size_t descriptorCount = descriptors.size()/descriptorDim;
		cout << descriptorCount << " descriptors extracted" << endl;

		size_t ndesc = rcf.ndesc;
		if(ndesc>descriptorCount) {
			cout << "Only " << descriptorCount << "/" << ndesc << " descriptors extracted\n";
			ndesc = descriptorCount;

			cout << "Save descriptors ... ";
			Matrix out(&descriptors[0], descriptorDim, descriptorCount);
			out.write(rcf.desc_output);
		}
		else {
			cout << "Save descriptors ... ";
			Matrix out(descriptorDim, ndesc, MATRIX_LOCAL);
			int b = descriptorCount/ndesc;
			for(int i=0; i<ndesc; i++) {
				memcpy(&out[i*descriptorDim], &descriptors[i*b*descriptorDim], descriptorDim*sizeof(float));
			}
			out.write(rcf.desc_output);
		}

		return 0;

	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}





///////////////////////////
// DESCRIPTORS CONQUEROR //
///////////////////////////

#include "retin/toolbox/core/thread_pool.h"
#include "retin/feature/depthimages/DepthImages.h"

/* local class to compute descriptors on all models */
class descriptor_conqueror: public thread_pool::conqueror {
private:
	vector<string> doc_list;
	vector<float>& descriptors;
	retin_config& rcf;

public:
	descriptor_conqueror(retin_config& config, vector<float>& desc): descriptors(desc), rcf(config){
		list_docs(rcf, doc_list);
	}
	void    process (size_t i) {
		if( i < doc_list.size()) {
			cout << "... " << doc_list[i] << " ..." << endl;

			float *buffer;
			size_t count;
			if(is_fvec_file(doc_list[i])) {
				if(!read_descriptors(doc_list[i], rcf, buffer, count)) {
					cerr << "Couldn't read descriptors from " << doc_list[i] << endl;
					return;
				}
			} else {
				if(!extract_descriptors(doc_list[i], rcf, buffer, count)) {
					cerr << "Couldn't extract descriptors for " << doc_list[i] << endl;
					return;
				}
			}

			lock();
			for(size_t i = 0 ; i < count ; i++) {
				for(size_t d = 0 ; d < 128 ; d++)
					descriptors.push_back(buffer[i*128+d]);
			}
			unlock();

		}
	}
};

bool load_descriptors(vector<float>& desc, retin_config& rcf) {

	thread_pool pool;
	pool.run();
	descriptor_conqueror dc(rcf, desc);
	dc.divide_and_conquer(&pool, 0, 10000);

	return true;
}
