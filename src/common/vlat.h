/*
 * vlat.h
 *
 *  Created on: 14 oct. 2013
 *      Author: jfellus
 */

#ifndef __VLAT_H_
#define __VLAT_H_

#include "common/config_reader.h"
#include "retin/toolbox/core/SharedMatrix.h"
#include "retin/feature/vlat/StandardVlatBuilder.h"
#include "retin/feature/vlat/StandardPrePcaVlatBuilder.h"

#include <string>

using namespace std;
using namespace shared_matrices;


class Codebook {
public:
	Codebook(retin_config& rcf);
	~Codebook();

	Matrix* centroids;
	Matrix* mean1;
	Matrix* mean2;

	size_t descDim;
	size_t K;

	void set_owner(bool bOwner = true) {
		centroids->set_owner(bOwner);
		mean1->set_owner(bOwner);
		mean2->set_owner(bOwner);
	}

	static bool free_shared(retin_config& rcf);
};


class VlatBuilder {
public:
	VlatBuilder(Codebook* codebook) {format = "LD";vlatDim=0;this->codebook = codebook;}
	virtual ~VlatBuilder() {}

	virtual void process(float* descriptors, size_t nbDesc) = 0;
	virtual void getVlat(float* buffer) = 0;

	string format;
	size_t vlatDim;
	Codebook* codebook;
};

VlatBuilder* create_vlat_builder(const retin_config& rcf, Codebook* codebook);


void compute_vlat_projectors(const retin_config& rcf, float* vlat, size_t n, size_t descriptorDim, float* proj, float* eigen_values);

void compute_gram(float*& gram, const float *vlat, const size_t featuredim, const size_t featureCount);
void compute_projectors(float* proj, const float *eigvectors, const float *eigvalues, const float *vlat, const size_t featureDim, const size_t featureCount, const size_t nbProjs);



float* vlat_compute(float* descriptors, size_t nbDescriptors, VlatBuilder* builder);
void vlat_normalize(float*& vlat, size_t vlatDim, float power_norm);
float* vlat_compact(float* vlat, float* proj, size_t dim, size_t nbProj);

float* compute_compact_vlat(float* query_descriptors, size_t nbDescriptors, VlatBuilder* builder, float* proj, size_t nbProj, const retin_config& rcf);


void preprocess_index(float* index, size_t indexDim, size_t n, float powernorm, bool normalize);
void preprocess_index(float* index, size_t indexDim, size_t n, const retin_config& rcf);
void preprocess_projectors(float* proj, size_t projDim, const retin_config& rcf);


float* matrix_crop(float* in, size_t w, size_t h, size_t nw, size_t nh);

#endif /* VLAT_H_ */
