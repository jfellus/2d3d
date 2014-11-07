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

#include "tools.h"
#include <dirent.h>
#include <exception>
#include <jpeglib.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>

void read_doc_list(const string& doc_listing, const string& doc_dir, vector<string>& doc_list);

void read_doc_list(const string& doc_listing, const string& doc_dir, vector<string>& doc_list) {
	ifstream in;
	in.open(doc_listing);
	if(in.is_open())
		while(in.good()) {
			string name;
			getline(in, name);
			if(name.empty()) continue;
			string fullname = doc_dir+"/"+name;
			doc_list.push_back(fullname);
		}
	else cout << "error opening " << doc_listing << endl;
}


void list_directory(const string& dir, vector<string>& doc_list, bool absolute_paths) {
	DIR   *d = opendir(dir.c_str());
	struct dirent *dd;

	if(!d) {
		cout << "Can't open " << dir << "\n";
		throw "Can't open directory";
	}
	while ((dd = readdir(d)) != NULL) {
		string fn = dd->d_name;
		if(fn == ".." || fn == ".") continue;

		if(absolute_paths)	doc_list.push_back(dir + "/" + fn);
		else doc_list.push_back(fn);
	}
	closedir(d);

	sort(doc_list.begin(),doc_list.end());
}

void list_docs(const retin_config& rcf, vector<string>& doc_list) {
	if(rcf.doc_list.empty()) {
		cout << "Read documents from " << rcf.doc_dir << endl;
		list_directory(rcf.doc_dir, doc_list);
	}
	else {
		cout << "Read documents from " << rcf.doc_list << endl;
		read_doc_list(rcf.doc_list, rcf.doc_dir, doc_list);
	}
}



void matrix_to_jpg(const float* matrix, size_t w, size_t h, const char* outfile) {
		struct jpeg_compress_struct cinfo;
		struct jpeg_error_mgr jerr;
		JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
		int row_stride;		/* physical row width in image buffer */

		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);

		FILE* f = fopen(outfile, "w");
		if(!f) throw "Can't open file";

		jpeg_stdio_dest(&cinfo, f);

		cinfo.image_width = w; 	/* image width and height, in pixels */
		cinfo.image_height = h;
		cinfo.input_components = 3;		/* # of color components per pixel */
		cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, 80, TRUE /* limit to baseline-JPEG values */);

		jpeg_start_compress(&cinfo, TRUE);
		row_stride = w * 3;

		row_pointer[0] = new JSAMPLE[row_stride];
		while (cinfo.next_scanline < cinfo.image_height) {
			for(int i=0; i<w; i++) {
				row_pointer[0][i*3] = (unsigned char) (matrix[cinfo.next_scanline * w * 3 + i*3]);
				row_pointer[0][i*3+1] = (unsigned char)(matrix[cinfo.next_scanline * w * 3 + i*3 +1]);
				row_pointer[0][i*3+2] = (unsigned char)(matrix[cinfo.next_scanline * w * 3 + i*3 +2]);
			}
			(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}
		delete row_pointer[0];

		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
}


void cmd(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char* s = new char[1000];
	vsprintf(s, fmt, ap);
	va_end(ap);

	if(system(s)) cerr << "Error executing " << s;
	delete[] s;
}

bool file_has_ext(const string& path, const char* ext) {
	return path.rfind(".ply") == path.length()-strlen(ext);
}
