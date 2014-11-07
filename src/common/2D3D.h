/*
 * 2D3D.h
 *
 *  Created on: 14 oct. 2013
 *      Author: jfellus
 */

#ifndef _2D3D_H_
#define _2D3D_H_

using namespace std;

#ifdef RETIN_3D
#include "retin/feature/depthimages/DepthImages.h"
#endif

#include "retin/toolbox/core/fvec_writer.h"
#include "retin/toolbox/core/fvec_reader.h"
#include <string>
#include "common/config_reader.h"

bool get_descriptors(const string& filename, const retin_config& rcf, float*& buffer, size_t& count);
bool is_fvec_file(const string& filename);

bool read_descriptors(const string& filename, const retin_config& rcf, float*& buffer, size_t& count);
bool extract_descriptors(const string& filename, const retin_config& rcf, float*& buffer, size_t& count);

void extract_descriptors_3d(const string& filename, const retin_config& rcf, float*& buffer, size_t& count);
void extract_descriptors_img(const string& filename, const retin_config& rcf, float*& buffer, size_t& count);




#endif /* _2D3D_H_ */
