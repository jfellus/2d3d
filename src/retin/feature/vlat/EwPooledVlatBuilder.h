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

#ifndef __vlat__EwPooledVlatBuilder_h__
#define __vlat__EwPooledVlatBuilder_h__

#include "VlatBuilder.h"

namespace vlat {

class EwPooledVlatCluster : public AbstractVlatCluster
{
protected:
        float   sign;
        size_t valuesCount;
        std::vector<float> maxValues;

        void    poolValues(const float* values);
        
public:
        EwPooledVlatCluster();        
        void    setSign(float sign_) { sign = sign_; }
        void    setCount(size_t valuesCount_) { valuesCount = valuesCount_; }
    
virtual void    init();
virtual void    add(const float* feature,size_t featureDim);
        size_t  getVlat(float* buf,const std::string& format) const;

};

class EwPooledVlatBuilder : public VlatBuilder<EwPooledVlatCluster>
{
protected:

public:
        EwPooledVlatBuilder(size_t order,size_t dictSize,size_t featureDim,float sign);
virtual ~EwPooledVlatBuilder();

        size_t  getVlatDim(const std::string& format) const;
        void    getVlat(float* buf,const std::string& format) const;
};


}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_EwPooledVlatBuilder_run
  (JNIEnv *, jobject, jobject, jint, jstring);

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_EwPooledVlatBuilder_runImage
  (JNIEnv *, jobject, jstring, jint, jstring, jstring, jobject);


}

#endif

#endif
