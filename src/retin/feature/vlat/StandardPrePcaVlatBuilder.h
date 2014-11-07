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

