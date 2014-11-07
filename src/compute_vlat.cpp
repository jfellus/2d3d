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


#include "common/2D3D.h"
#include "common/vlat.h"
#include "common/tools.h"


using namespace std;
using retin::fvec_writer;
using vlat::StandardVlatBuilder;
using vlat::StandardPrePcaVlatBuilder;

#ifdef RETIN_3D
	using retin::DepthImages;
#endif

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
	cout << "pre_pca: " << rcf.pre_pca << "\n" << endl;
}




//! Program that computes vlat signatures on 3D models
int main(int argc, char **argv) {
	try {
		string config_file = "config.properties";
		if(argc > 1) config_file = argv[1];
		cout << "config: " << config_file << endl;

		retin_config rcf = read_config_file(config_file);
		dump_config(rcf);

		// Load codebook
		Codebook* codebook = new Codebook(rcf);

		VlatBuilder* builder = create_vlat_builder(rcf, codebook);

		// read all entries of the file and push into list
		vector<string> doc_list;
		list_docs(rcf, doc_list);

		fvec_writer<float>* writer = NULL;
		ofstream file;
		Matrix* index = NULL;
		if(rcf.vlat_inmemory) {
			index = new Matrix(builder->vlatDim,doc_list.size(), rcf.vlat_file);
		} else {
			file.open(rcf.vlat_file.c_str(),std::ios::binary);
			if (file.fail()) {
				cout << "Error " << rcf.vlat_file << endl;
				return -1;
			}
			writer = new fvec_writer<float>(file);
		}

		float *vlat = new float[builder->vlatDim];
		float *buffer = NULL;
		size_t count;

		cout << "computing " << (rcf.pre_pca ? "cluster wise " : "") << "vlat" << endl;

		for(size_t i = 0 ; i < doc_list.size() ; i++) {
			cout << "... " << doc_list[i] << " (" << i << "/" << doc_list.size() << ")";

			// read/extract descriptors
			if(!get_descriptors(doc_list[i], rcf, buffer, count)) continue;

			// compute vlat
			builder->process(buffer, count);

			if(buffer) { delete [] buffer; buffer = NULL; }

			if(rcf.vlat_inmemory) builder->getVlat(index->get_row(i));
			else {
				builder->getVlat(vlat);
				writer->write_feature(vlat,builder->vlatDim);
			}

			cout << endl;
		}
		cout << "done" << endl;

		if(index) delete index;
		if(writer) delete writer;
		if(file.good()) file.close();

		return 0;
	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
