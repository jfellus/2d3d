/*
 * 2D3D.cpp
 *
 *  Created on: 14 oct. 2013
 *      Author: jfellus
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
