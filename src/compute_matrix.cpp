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

using namespace std;

#include "common/config_reader.h"
#include "retin/toolbox/core/SharedMatrix.h"
#include "retin/toolbox/algebra/vector_float.h"
#include "common/2D3D.h"


using retin::fvec_writer;
using namespace shared_matrices;



void dump_config(const retin_config& rcf) {
	cout << "feature file: " << rcf.feature_file << endl;
	cout << "power norm: " << rcf.feature_pn << endl;
}


//! program that computes a distance matrix shrec format
int main(int argc, char **argv) {
	try {
		string config_file = "config.properties";
		if(argc > 1) config_file = argv[1];
		cout << "config: " << config_file << endl;
		retin_config rcf = read_config_file(config_file);
		dump_config(rcf);

		cout << "loading vlat ... ";
		Matrix vlat(rcf.feature_file);
		if(!vlat) {	cout << "Error " << rcf.vlat_file << endl;	exit(-1); }
		cout << vlat.height << " features of size "<< vlat.width << endl;

		cout << "power normalization ... ";
		vector_pow_float(vlat, rcf.feature_pn, vlat.height*vlat.width);
		cout << "ok\n";
		cout << "l2 normalization ... ";
		for(size_t i = 0 ; i < vlat.height ; i++) {
			float n2 = vector_n2_float(vlat.get_row(i), vlat.width);
			vector_sdiv_float(vlat.get_row(i), n2, vlat.width);
		}
		cout << "ok\n";

		cout << "compute distance matrix ... ";
		Matrix matrix(vlat.height, vlat.height, rcf.matrix_file);
		for(size_t i = 0 ; i < vlat.height ; i++) {
			float *v1 = vlat.get_row(i);
			for(size_t j = i ; j < vlat.height ; j++) {
				float *v2 = vlat.get_row(j);
				matrix[i*vlat.height+j] = vector_l2_float(v1, v2, vlat.width);
				matrix[j*vlat.height+i] = matrix[i*vlat.height+j];
			}
		}
		cout << "ok\n";

		// SAVE AS FVEC
		matrix.save(rcf.matrix_file);


		/*
	// SAVE AS TXT

	ofstream file;
	file.open(rcf.matrix_file);
	if (file.fail()) {
		cout << "Error " << rcf.matrix_file << endl;
		return -1;
	}

	for(size_t i = 0 ; i < n ; i++) {
		for(size_t j = 0 ; j < n ; j++) {
			file << matrix[i*n+j] << " ";
		}
		file << endl;
	}
		 */

		cout << "finished" << endl;


		return 0;

	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
