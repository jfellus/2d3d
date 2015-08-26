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
/* 
 * File:   SparseVlatBuilder.h
 * Author: romanegr
 *
 * Created on 28 mai 2013, 11:19
 */

#ifndef __vlat__SPARSEVLATBUILDER_H__
#define	__vlat__SPARSEVLATBUILDER_H__

#include "StandardVlatBuilder.h"

namespace vlat {
    
class SparseVlatCluster : public StandardVlatCluster
{
public:
    static const int sparsityOnDiag = 0;
    static const int sparsityOnFull = 1;
    
    static const int sparsityStrategyDim = 0;
    static const int sparsityStrategyEnergy = 1;
    static const int sparsityMaxValues = 2;
    
protected:
    int sparsityOn;
    int sparsityStrategy;
    size_t sparsityDim;
    float sparsityEnergy;
    
    std::vector<float> L;
    std::vector<float> sqrtinvL;
    std::vector<float> U;
    
    std::vector<float> varDual;
    
    size_t counter;

public:
    SparseVlatCluster();
    
    void setSparsityOn(int o);
    void setSparsityStrategy(int s);
    void setEnergySparsity(float e);
    void setDimSparsity(size_t d);
    
    virtual void setMainOrder(size_t dim);
    
    virtual void init();
    virtual void add(const float* feature,size_t featureDim);
    virtual size_t getVlat(float* buf,const std::string& format) const;
    
    virtual void setEigenDecomposition(const float* eigenVectors , const float* eigenValues, size_t vectorsDim, size_t vectorsSize);
};

class SparseVlatBuilder : public VlatBuilder<SparseVlatCluster> {
public:
    SparseVlatBuilder(size_t order,size_t dictSize,size_t featureDim);
    virtual ~SparseVlatBuilder();
    
    void setSparsityOn(int o);
    void setSparsityStrategy(int s);
    void setEnergySparsity(float e);
    void setDimSparsity(size_t d);

    size_t getVlatDim(const std::string& format) const;
    void getVlat(float* buffer,const std::string& format) const;
    virtual void setEigenDecomposition(size_t cluster, const float* eigenVectors , const float* eigenValues, size_t vectorsDim, size_t vectorsSize);
private:

};
    
}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_SparseVlatBuilder_run
  (JNIEnv *, jobject, jobject, jint, jstring);

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_SparseVlatBuilder_runNativeGenerator
  (JNIEnv *, jobject, jobject, jstring, jint, jstring);


}

#endif /* RETIN_DISABLE_JAVA */


#endif	/* __vlat__SPARSEVLATBUILDER_H__ */
