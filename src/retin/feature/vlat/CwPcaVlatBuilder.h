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

#ifndef __vlat__CwPcaVlatBuilder_h__
#define __vlat__CwPcaVlatBuilder_h__

#include "StandardVlatBuilder.h"

#include <vector>

namespace vlat {

class PcaProjector
{
    size_t  featureDim,dirCount;
    const float* dir;
    const float* mean;
    const float* var;
    float* meanTensor2;
public:
    PcaProjector();
    ~PcaProjector();

    void setParams(const float* dir,size_t dirsCount,size_t dirsDim,
        const float* mean,size_t meansDim,const float* var,size_t varDim);
    size_t  getProjectSize() const { return dirCount; }
    void    project(float* output,const float* input,size_t inputDim) const;
    const float* getMeanTensor2() const {
        return meanTensor2;
    }
};

class CwPcaVlatBuilder : public StandardVlatBuilder
{
protected:
        std::vector<PcaProjector> projectors;

public:
        CwPcaVlatBuilder(size_t order,size_t dictSize,size_t featureDim);

        void setPcaParams(size_t cluster,
            const float* dir,size_t dirCount,size_t dirDim,
            const float* mean,size_t meanDim,const float* var,size_t varDim);


virtual void add (size_t cluster,const float* data,size_t dim);

};


}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPcaVlatBuilder_compDict
  (JNIEnv *, jobject, jobject, jobjectArray, jint, jdouble);

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPcaVlatBuilder_compMeanVlat
  (JNIEnv *, jobject, jobject, jobjectArray, jint, jstring);

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPcaVlatBuilder_compVlat
  (JNIEnv *, jobject, jobject, jint, jstring);

}

#endif

#endif
