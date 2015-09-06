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


#ifndef _config_reader_h_
#define _config_reader_h_

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"

using namespace std;

class retin_config {
public:

	// Documents
	string doc_list;
	string doc_dir;
	string desc_output;
	size_t ndesc;

	// Codebook
	size_t dict_size;
	string dict_dir;
	string dict_file;
	string mean1_file;
	string mean2_file;

	// Codebook pre-PCA
	bool   pre_pca;
	string pre_pca_strategy;
	size_t pre_pca_dim;
	float pre_pca_energy;

	// Image/3D descriptors
	string desc_type;
	vector<size_t> desc_scales;
	size_t desc_step;
	int desc_minX,desc_minY,desc_maxX,desc_maxY;

	// 3D descriptors
	size_t nb_views;

	// VLAT
	float  power_norm;
	string vlat_file;
	float feature_pn;
	bool vlat_inmemory;

	// COMPACT-VLAT
	string proj_dir;
	string weight_file;
	string proj_file;
	size_t nb_proj;
	bool   proj_white;
	string compact_file;
	int proj_learning_set_fraction;
	bool release_vlat_after_compact;

	// INDEX
	string feature_file;
	string matrix_file;

	// QUERY
	string similarity_file;
	string query_file;

	string thumbnails_dir;

	float similarity_score_sigmoid_factor;

	bool transpose_proj;

	retin_config(): dict_size(8), desc_step(8), nb_views(10), 
					doc_list("documents.txt"), doc_dir("documents"), 
					dict_file("dict.fvec"), mean1_file("mean1.fvec"), 
					mean2_file("mean2.fvec"), dict_dir("dict"),
					pre_pca(false), pre_pca_strategy("dim"),
					pre_pca_dim(128), pre_pca_energy(1.0f),
					vlat_file("output.fvec"), power_norm(0.5f),
					proj_dir("proj"),
					weight_file("weights.fvec"), proj_file("proj.fvec"),
					nb_proj(128), proj_white(false),
					compact_file("compact.fvec"),
					feature_file(vlat_file), matrix_file("resultMatrix.txt"),
					feature_pn(1.0f),
					desc_minX(0),desc_minY(0),desc_maxX(0),desc_maxY(0),
					desc_type("hog"),similarity_file("query_similarity.txt"),query_file("query.fvec"),
					proj_learning_set_fraction(100),
					vlat_inmemory(true), release_vlat_after_compact(true),
					thumbnails_dir("thumbnails"),
					similarity_score_sigmoid_factor(0.3f),
					desc_output("descriptors.fvec"),ndesc(5000000),
					transpose_proj(false)
					{
		desc_scales.push_back(8);
	};


};

retin_config read_config_file(string file);

#pragma GCC diagnostic pop

#endif
