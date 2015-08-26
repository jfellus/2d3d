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

#include "vlat.h"

using vlat::StandardVlatBuilder;
using vlat::StandardPrePcaVlatBuilder;

#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"


Codebook::Codebook(retin_config& rcf) {
	cout << "load codebook ... ";

	centroids = new Matrix(rcf.dict_dir+"/"+rcf.dict_file);
	if (!*centroids) {	cout << "Error : no codebook " << rcf.dict_dir+"/"+rcf.dict_file << endl; exit(-1); }

	K = centroids->height;
	descDim = centroids->width;

	mean1 = new Matrix(rcf.dict_dir+"/"+rcf.mean1_file);
	if (!*mean1) {	cout << "Error : no mean tensor 1 " << rcf.dict_dir+"/"+rcf.mean1_file << endl;	exit(-1);}

	mean2 = new Matrix(rcf.dict_dir+"/"+rcf.mean2_file);
	if (!*mean2) {	cout << "Error : no mean tensor 2 " << rcf.dict_dir+"/"+rcf.mean2_file << endl;	exit(-1);}

	cout << " ok" << endl;
}

Codebook::~Codebook() {
	delete centroids;
	delete mean2;
	delete mean1;
}


bool Codebook::free_shared(retin_config& rcf) {
	Matrix::free_shared(rcf.dict_dir+"/"+rcf.dict_file);
	Matrix::free_shared(rcf.dict_dir+"/"+rcf.mean1_file);
	Matrix::free_shared(rcf.dict_dir+"/"+rcf.mean2_file);
	return true;
}


class PrePcaVlatBuilder : public VlatBuilder {
public:
	StandardPrePcaVlatBuilder* builder;

	PrePcaVlatBuilder(const retin_config& rcf, Codebook* codebook);

	virtual void process(float* descriptors, size_t nbDesc) {
		cout << ".";
		builder->process(descriptors, nbDesc, codebook->descDim);
	}

	virtual void getVlat(float* buffer) {
		cout << ".";
		builder->getVlat(buffer, format);
		cout << ".";
	}
};


class NoPcaVlatBuilder : public VlatBuilder {
public:
	StandardVlatBuilder* builder;

	NoPcaVlatBuilder(const retin_config& rcf, Codebook* codebook);

	virtual void process(float* descriptors, size_t nbDesc) {
		cout << ".";
		builder->process(descriptors, nbDesc, codebook->descDim);
	}

	virtual void getVlat(float* buffer) {
		cout << ".";
		builder->getVlat(buffer, format);
		cout << ".";
	}
};


PrePcaVlatBuilder::PrePcaVlatBuilder(const retin_config& rcf, Codebook* codebook) : VlatBuilder(codebook) {
	builder = new StandardPrePcaVlatBuilder(2, codebook->K, codebook->descDim);

	builder->setCenters(*(codebook->centroids), codebook->K, codebook->descDim);
	builder->setMeanTensors(1, *(codebook->mean1), codebook->K, codebook->descDim);
	builder->setMeanTensors(2, *(codebook->mean2),  codebook->K, codebook->descDim*codebook->descDim);

	builder->setDimThreshold(rcf.pre_pca_dim);
	builder->setEnergyThreshold(rcf.pre_pca_energy);
	builder->setWhitening(false);
	if(rcf.pre_pca_strategy == "dim"){
		cout << "pre_pca_dim: " << rcf.pre_pca_dim << endl;
		builder->setThresholdingStrategy(vlat::StandardPrePcaVlatCluster::thresholdingStrategyDim);
	}
	else {
		cout << "pre_pca_energy: " << rcf.pre_pca_energy << endl;
		builder->setThresholdingStrategy(vlat::StandardPrePcaVlatCluster::thresholdingStrategyEnergy);
	}
	builder->computeDimDual();
	vlatDim = builder->getVlatDim(format);
	cout << "vlat dimension: " << vlatDim << endl;
}

NoPcaVlatBuilder::NoPcaVlatBuilder(const retin_config& rcf, Codebook* codebook) : VlatBuilder(codebook) {
	builder = new StandardVlatBuilder(2, codebook->K, codebook->descDim);
	builder->setCenters(*(codebook->centroids), codebook->K, codebook->descDim);
	builder->setMeanTensors(1, *(codebook->mean1), codebook->K, codebook->descDim);
	builder->setMeanTensors(2, *(codebook->mean2), codebook->K, codebook->descDim*codebook->descDim);
	vlatDim = builder->getVlatDim(format);
	cout << "vlat dimension: " << vlatDim << endl;
}



VlatBuilder* create_vlat_builder(const retin_config& rcf, Codebook* codebook) {
	return rcf.pre_pca ? (VlatBuilder*) new PrePcaVlatBuilder(rcf, codebook) : (VlatBuilder*) new NoPcaVlatBuilder(rcf, codebook);
}




#include "retin/toolbox/core/thread_pool.h"



// ----------------------
// threading gram computation

class gram_conqueror : public thread_pool::conqueror {
protected:
	float* gram;
	const float* vlat;
	const size_t featureDim;
	const size_t featureCount;

public:
	gram_conqueror(float *g, const float *v, const size_t d, const size_t c) : gram(g), vlat(v), featureDim(d), featureCount(c) {}

	void process(size_t i) {
		if(i < featureCount) {
			printf(".");fflush(stdout);

			for(size_t j = i ; j < featureCount ; j++) {
				gram[i*featureCount+j] = vector_ps_float(vlat+i*featureCount, vlat+j*featureCount, featureDim);
				gram[j*featureCount+i] = gram[i*featureCount+j];
			}
		}
	}

};

float* compute_gram(const float *vlat, const size_t featureDim, const size_t featureCount) {
	cout << "computing gram ... " << endl;

	thread_pool pool;
	pool.run();

	float* gram = new float[featureCount*featureCount];

	gram_conqueror gc(gram, vlat, featureDim, featureCount);
	gc.divide_and_conquer(&pool, 0, featureCount);
	cout << "ok\n";

	return gram;
}



// ------------------------------------
// threading projector computation

class projectors_conqueror : public thread_pool::conqueror {
protected:
	float* proj;
	const float *eigvectors;
	const float *eigvalues;
	const float *vlat;
	const size_t featureDim;
	const size_t featureCount;

public:
	projectors_conqueror(float* p, const float *ee, const float *ea, const float *v, const size_t d, const size_t c) :
							proj(p), eigvectors(ee), eigvalues(ea), vlat(v), featureDim(d), featureCount(c) {}

	void process(size_t i) {
		printf("."); fflush(stdout);
		if(i < featureCount) {
			float * monproj = proj+i*featureDim;
			memset(monproj, 0, featureDim*sizeof(float));
			matrix_CpAB_float(monproj, vlat, eigvectors+i*featureCount, featureDim, featureCount, 1);
			vector_smul_float(monproj, 1/sqrtf(eigvalues[i]), featureDim);
		}
	}

};

void compute_projectors(const float *eigvectors, const float *eigvalues, const float *vlat, const size_t featureDim, const size_t featureCount, const size_t nbProjs, float* proj) {
	cout << "computing projectors ... ";

	thread_pool pool;
	pool.run();

	projectors_conqueror pc(proj, eigvectors, eigvalues, vlat, featureDim, featureCount);
	pc.divide_and_conquer(&pool, 0, nbProjs);
	delete vlat;

	cout << "ok\n";
}

void compute_vlat_projectors(const retin_config& rcf, float* vlat, size_t n, size_t vlatDim, float* proj, float* eigen_values) {
	cout << "power normalization ... ";
	vector_pow_float(vlat, rcf.power_norm, n*vlatDim);
	cout << "ok\n";

	cout << "l2 normalization ... ";
	for(size_t i = 0 ; i < n ; i++) {
		float n2 = vector_n2_float(vlat+i*vlatDim, vlatDim);
		vector_sdiv_float(vlat+i*vlatDim, n2, vlatDim);
	}
	cout << "ok\n";

	// compute Gram matrix
	float *gram = compute_gram(vlat, vlatDim, n);

	// eig
	cout << "Eigenvalues ... ";
	matrix_eigSym_float(gram, eigen_values, n);
	matrix_sortEig_float(gram, eigen_values, n);
	cout << "ok\n";


	// compute projectors
	compute_projectors(gram, eigen_values, vlat, vlatDim, n, rcf.nb_proj, proj);
}




float* vlat_compute(float* descriptors, size_t nbDescriptors, VlatBuilder* builder) {
	size_t vlatDim = builder->vlatDim;
	float* vlat = new float[vlatDim];
	builder->process(descriptors, nbDescriptors);
	builder->getVlat(vlat);
	return vlat;
}

float* vlat_compact(float* vlat, float* proj, size_t dim, size_t nbProj) {
	cout << "projecting on " << nbProj << " projectors ... ";
	float* compact_vlat = new float[nbProj];
	memset(compact_vlat, 0, nbProj);
	matrix_CpAtB_float(compact_vlat, proj, vlat, nbProj, dim, 1);
	cout << "ok\n";
	return compact_vlat;
}

void vlat_normalize(float*& vlat, size_t vlatDim, float power_norm) {
	cout << "power normalization ... ";
	vector_pow_float(vlat, power_norm, vlatDim);
	cout << "ok\n";

	cout << "l2 normalization ... ";
	float n2 = vector_n2_float(vlat, vlatDim);
	vector_sdiv_float(vlat, n2, vlatDim);
	cout << "ok\n";
}

float* compute_compact_vlat(float* query_descriptors, size_t nbDescriptors, VlatBuilder* builder, float* proj, size_t nbProj, const retin_config& rcf) {
	float* vlat = vlat_compute(query_descriptors, nbDescriptors, builder);
	vlat_normalize(vlat, builder->vlatDim, rcf.power_norm);
	return vlat_compact(vlat, proj, builder->vlatDim, nbProj);
}



/////////////


void preprocess_index(float* index, size_t indexDim, size_t n, float powernorm, bool normalize) {
	cout << "index power normalization ... ";
	vector_pow_float(index, powernorm, n*indexDim);
	cout << "ok\n";

	if(normalize) {
	cout << "index l2 normalization ... ";
	for(size_t i = 0 ; i < n ; i++) {
		float n2 = vector_n2_float(index+i*indexDim, indexDim);
		vector_sdiv_float(index+i*indexDim, n2, indexDim);
	}
	cout << "ok\n";
	}
}

void preprocess_index(float* index, size_t indexDim, size_t n, const retin_config& rcf) {
	preprocess_index(index, indexDim, n, rcf.feature_pn, true);
}

void preprocess_projectors(float* proj, size_t projDim, const retin_config& rcf) {
	if(rcf.proj_white) {
		cout << "whitening projectors ... ";
		Matrix values(rcf.proj_dir+"/"+rcf.weight_file, true);
		if (!values) {	cout << "Error " << rcf.weight_file << endl; exit(-1);	}

		for(size_t j = 0 ; j < values.width ; j++) {
			float s = 1.f/sqrtf(values[j]);
			vector_smul_float(proj+j*projDim, s, projDim);
		}
		cout << "ok\n";
	}
}

float* matrix_crop(float* in, size_t w, size_t h, size_t nw, size_t nh) {
	float* out = new float[nw*nh];
	for(size_t i=0; i<nh; i++) {
		for(size_t j=0; j<nw; j++) {
			out[i*nw+j] = in[i*w+j];
		}
	}
	return out;
}

