/*
 * main.cpp
 *
 *  Created on: 25 oct. 2013
 *      Author: jfellus
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
