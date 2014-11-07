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



#include "2D3D.h"
#include "retin/feature/depthimages/DepthImages.h"
#include "retin/feature/dfeat/DaveHog.h"
#include "retin/feature/dfeat/DaveDSift.h"
#include "retin/feature/dfeat/DaveFastSift.h"
#include <memory>
#include <auto_ptr.h>


using namespace std;



void extract_descriptors_3d(const string& filename, const retin_config& rcf, float*& buffer, size_t& count) {
#ifdef RETIN_3D
	retin::DepthImages di;
	di.setStep(rcf.desc_step);
	di.setNbCams(rcf.nb_views);
	di.setScales(((retin_config&)rcf).desc_scales);
	di.extractDescriptors(buffer, filename, count);
#else
	fprintf(stderr, "3D DESCRIPTORS DISABLED !!\n");
#endif
}

void extract_descriptors_img(const string& filename, const retin_config& rcf, float*& buffer, size_t& count) {
	auto_ptr<dfeat::AbstractDaveFeature> extractor;

	if (rcf.desc_type == "hog") extractor.reset(new dfeat::DaveHog());
	else if (rcf.desc_type == "dsift") extractor.reset(new dfeat::DaveDSift());
	else if (rcf.desc_type == "fastsift") extractor.reset( new dfeat::DaveFastSift());
	else {
		cout << filename << ": invalid descriptor type " << rcf.desc_type << endl;
		return;
	}

	try {
		extractor->setFileInput(filename.c_str());
		if(rcf.desc_maxX && rcf.desc_maxY) extractor->cropInput(rcf.desc_minX,rcf.desc_minY,rcf.desc_maxX,rcf.desc_maxY);

		buffer = NULL;
		dfeat::extractDaveFeature (buffer,count,*extractor,rcf.desc_step,rcf.desc_scales);

	}
	catch (exception& ex) {	cerr << filename << ": " << ex.what() << endl;}
}

bool get_descriptors(const string& filename, const retin_config& rcf, float*& buffer, size_t& count) {
	if(is_fvec_file(filename)) {
		if(!read_descriptors(filename, rcf, buffer, count)) return false;
	}
	else if(!extract_descriptors(filename, rcf, buffer, count)) return false;
	return true;
}



bool is_fvec_file(const string& filename) {
	return( retin::string_has_file_ext(filename, ".fvec")
	|| 	retin::string_has_file_ext(filename, ".fvecs")
	|| 	retin::string_has_file_ext(filename, ".ivec")
	|| 	retin::string_has_file_ext(filename, ".ivecs")
	|| 	retin::string_has_file_ext(filename, ".hvec8")
	|| 	retin::string_has_file_ext(filename, ".peter8")
	|| 	retin::string_has_file_ext(filename, ".csv")
	|| 	retin::string_has_file_ext(filename, ".txt")
	|| 	retin::string_has_file_ext(filename, ".m")
	|| 	retin::string_has_file_ext(filename, ".bin16")
	|| 	retin::string_has_file_ext(filename, ".siftgeo")
	);
}

bool read_descriptors(const string& filename, const retin_config& rcf, float*& buffer, size_t& count) {
	size_t dim;
	return read_fvec_file(filename, buffer, dim, count);
}

bool extract_descriptors(const string& filename, const retin_config& rcf, float*& buffer, size_t& count) {
	if(		retin::string_has_file_ext(filename, ".ply")
	|| 	retin::string_has_file_ext(filename, ".off")
	|| 	retin::string_has_file_ext(filename, ".obj")
	) extract_descriptors_3d(filename, rcf, buffer, count);
	else if(retin::string_has_file_ext(filename, ".jpg")
	|| 	retin::string_has_file_ext(filename, ".png")
	|| 	retin::string_has_file_ext(filename, ".pgm")
	) extract_descriptors_img(filename, rcf, buffer, count);
	else {
		std::cerr << "Unrecognized file format : " << filename << std::endl;
		return false;
	}
	return true;
}
