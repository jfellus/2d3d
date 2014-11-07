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

using namespace std;

#include "common/vlat.h"
#include "retin/toolbox/core/SharedMatrix.h"


//! Program that compact a set of vlat signatures
int main(int argc, char **argv) {
	try {
	string config_file = "config.properties";
	if(argc > 1) config_file = argv[1];
	cout << "config: " << config_file << endl;
	retin_config rcf = read_config_file(config_file);
	
	Codebook::free_shared(rcf);
	Matrix::free_shared(rcf.vlat_file);
	Matrix::free_shared(rcf.compact_file);
	Matrix::free_shared(rcf.proj_dir + "/" + rcf.proj_file);
	Matrix::free_shared(rcf.proj_dir + "/" + rcf.weight_file);
	Matrix::free_shared(rcf.feature_file);

	cout << "finished" << endl;
	return 0;
	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
