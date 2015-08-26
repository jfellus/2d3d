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

#ifndef __quantization__ElbgQuantizer_h__
#define __quantization__ElbgQuantizer_h__

#include "NaiveSplittingQuantizer.h"

namespace quantization {

class ElbgQuantizer : public NaiveSplittingQuantizer
{
protected:

    bool    swap(size_t l, size_t i, size_t p);
    void    fillEmptyClusters();
    void    optimize();
    int     findCenterP(size_t l);
    int     findCenterL(size_t i);

public:
    ElbgQuantizer(size_t featureDim) : NaiveSplittingQuantizer(featureDim) {}

    virtual void    run(const float* data,size_t dataDim,size_t dataCount);
    virtual void    computeClusters();
    virtual void    computeCenters();
    virtual void    removeCluster (size_t i,size_t l);
    virtual void    removeEmptyCluster (size_t i);

};

}

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_release
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runFeatureList
  (JNIEnv *, jobject, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runFeatureMatrix
  (JNIEnv *, jobject, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runTable
  (JNIEnv *, jobject, jobject, jobjectArray);

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runNativeGeneratorTable
  (JNIEnv *, jobject, jobject, jobject, jobjectArray);

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runSampledTable
  (JNIEnv *, jobject, jobject, jobjectArray, jint);

JNIEXPORT jdouble JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getDistortion
  (JNIEnv *, jobject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterCenters
  (JNIEnv *, jobject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterTensors
  (JNIEnv *, jobject, jint);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterSizes
  (JNIEnv *, jobject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterDistortions
  (JNIEnv *, jobject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterVariances
  (JNIEnv *, jobject);

JNIEXPORT jintArray JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getLabels
  (JNIEnv *, jobject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getIndexMatrix
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getFeatureMatrix
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getGaussianWeights
(JNIEnv * env, jobject thisObject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getGaussianMeans
(JNIEnv * env, jobject thisObject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getGaussianVars
(JNIEnv * env, jobject thisObject);

}

#endif

#endif
