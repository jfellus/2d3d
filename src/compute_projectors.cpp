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

#include "retin/toolbox/core/fvec_reader.h"
#include "retin/toolbox/core/fvec_writer.h"
#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"
#include "common/2D3D.h"
#include "common/vlat.h"
#include <sys/stat.h>

using namespace std;
using retin::fvec_writer;



void dump_config(const retin_config& rcf) {
	cout << "vlat file: " << rcf.vlat_file << endl;
	cout << "power norm: " << rcf.power_norm << "\n" << endl;
}


//! Program that computes vlat signatures on 3D models
int main(int argc, char **argv) {
	try {
		string config_file = "config.properties";
		if(argc > 1) config_file = argv[1];
		cout << "config: " << config_file << endl;

		retin_config rcf = read_config_file(config_file);
		dump_config(rcf);

		// load vlat
		cout << "loading vlat ... ";
		Matrix vlat_full(rcf.vlat_file, MATRIX_LOCAL, 2000);
		if (!vlat_full) {cout << "Error : no index in " << rcf.vlat_file << endl; exit(-1);	}
		cout << vlat_full.height << " features of size "<< vlat_full.width << endl;


		// Partial set learning
		if(!rcf.proj_learning_set_fraction) rcf.proj_learning_set_fraction = 100;
		size_t n = vlat_full.height*rcf.proj_learning_set_fraction/100;
		float* vlat = matrix_crop(vlat_full, vlat_full.width, vlat_full.height, vlat_full.width, n);

		if(n<rcf.nb_proj) {
			cout << "Warning : Projectors learning set is to small (required at least " << rcf.nb_proj << " but got " << n << ") -> setting to " << rcf.nb_proj << "\n";
			n = rcf.nb_proj;
		}
		cout << "Keep " << n << " signatures to learn projectors\n";

		Matrix proj(vlat_full.width, rcf.nb_proj, rcf.proj_dir+"/"+rcf.proj_file);
		Matrix eigen_values(rcf.nb_proj, 1, rcf.proj_dir+"/"+rcf.weight_file);

		compute_vlat_projectors(rcf, vlat, n, vlat_full.width, proj, eigen_values);

		mkdir(rcf.proj_dir.c_str(), 0755);

		// save all
		cout << "Save weights ... ";
		eigen_values.save(rcf.proj_dir+"/"+rcf.weight_file);
		cout << "ok\n";

		cout << "Save projectors ... " << endl;
		proj.save(rcf.proj_dir+"/"+rcf.proj_file);
		cout << "ok\n";



		fflush(stdout);
	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}


