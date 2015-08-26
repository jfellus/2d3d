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
 * File:   StandardPrePcaVlatBuilder.h
 * Author: romanegr
 *
 * Created on 24 octobre 2012, 15:10
 */

#ifndef __vlat__StandardPrePcaVlatBuilder_h__
#define	__vlat__StandardPrePcaVlatBuilder_h__

#include "StandardVlatBuilder.h"

namespace vlat {
    
class StandardPrePcaVlatCluster : public StandardVlatCluster
{
public:
    static const int thresholdingStrategyDim = 1;
    static const int thresholdingStrategyEnergy = 2;
    
protected:
    int thresholdingStrategy;
    size_t thresholdDim;
    float thresholdEnergy;
    float whitening;
    
    std::vector<float> L;
    std::vector<float> sqrtinvL;
    std::vector<float> U;
    
    size_t dualDim;

public:
    StandardPrePcaVlatCluster();
    
    void setThresholdingStrategy(int s);
    void setEnergyThreshold(float e);
    void setDimThreshold(size_t d);
    void setWhitening(bool w);
    
    void computeDimDual();
    
    virtual void setMainOrder(size_t dim);
    
    virtual void init();
    virtual void add(const float* feature,size_t featureDim);
    virtual size_t getVlat(float* buf,const std::string& format) const;
    
    virtual void setMeanTensor(size_t order,const float* meanTensor,size_t meanTensorDim);
        
};

class StandardPrePcaVlatBuilder : public VlatBuilder<StandardPrePcaVlatCluster> {
public:
    StandardPrePcaVlatBuilder(size_t order,size_t dictSize,size_t featureDim);
    virtual ~StandardPrePcaVlatBuilder();
    
    void computeDimDual();
    
    size_t getVlatDim(const std::string& format) const;
    
    void setThresholdingStrategy(int s);
    void setEnergyThreshold(float e);
    void setDimThreshold(size_t d);
    void setWhitening(bool w);

    void getVlat(float* buffer,const std::string& format) const;
private:

};

}

#endif	/* __vlat__StandardPrePcaVlatBuilder_h__ */
