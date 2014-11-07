/*
 * tools.h
 *
 *  Created on: 14 oct. 2013
 *      Author: jfellus
 */

#ifndef __TOOLS_H_
#define __TOOLS_H_

#include <string>
#include <vector>
#include "common/config_reader.h"


using namespace std;

void list_docs(const retin_config& rcf, vector<string>& doc_list);
void list_directory(const string& dir, vector<string>& doc_list, bool absolute_paths = true);

void matrix_to_jpg(const float* matrix, size_t w, size_t h, const char* outfile);

void cmd(const char* fmt, ...);


bool file_has_ext(const string& path, const char* ext);

#endif /* TOOLS_H_ */
