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

#define DBG(x) std::cout << x << "\n"

#define DBGN(x) \
		do { \
			std::cout << x << "\n"; fflush(stdout); \
			std::cout  << "\033[1F\033[2K"; \
		}while(0);

#define DBG_PERCENT(x) DBGN((int)((float)x*1000)/10.0f << "%")


#endif /* TOOLS_H_ */
