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

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <errno.h>
#include "common/tools.h"
#include <stdlib.h>
#include <unistd.h>

using namespace std;


void usage() {
	printf("Reads a documents ids stream and convert them to paths, eventually adding the given prefix path");
	printf("Usage : docid_to_path <path_to_thumbnails> <output_prefix> [config_file]\n");
	exit(-1);
}


vector<string> docs;


int main(int argc, char **argv) {
	try{
		string config_file = "config.properties";
		if(argc > 3) config_file = argv[3];
		retin_config rcf = read_config_file(config_file);

		string thumbnails_dir = "";
		if(argc > 1) thumbnails_dir = argv[1];

		list_directory(thumbnails_dir + "/" + rcf.thumbnails_dir, docs, false);

		string prefix = "";
		if(argc > 2) {	prefix = argv[2];  prefix += "/"; }

		string s;
		while(getline(std::cin,s).good()) {
			std::size_t found = s.find('=');
			int id = atoi(s.substr(0,found).c_str());
			float sim = atof(s.substr(found+1, s.length()).c_str());
			cout << prefix << docs[id] << ",id=" << id << ",score=" << sim << "%" << endl;
		}

	}catch(const char* msg) {
		cout << "FATAL ERROR : " << msg << "\n";
		return 1;
	}
}
