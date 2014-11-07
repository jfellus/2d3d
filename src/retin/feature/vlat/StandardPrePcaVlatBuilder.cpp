/* 
 * File:   StandardPrePcaVlatBuilder.cpp
 * Author: romanegr
 * 
 * Created on 24 octobre 2012, 15:10
 */

#include "StandardPrePcaVlatBuilder.h"

#include "retin/toolbox/algebra/matrix_float.h"

using namespace std;

namespace vlat {
    
StandardPrePcaVlatCluster::StandardPrePcaVlatCluster() : StandardVlatCluster() {
    thresholdingStrategy = StandardPrePcaVlatCluster::thresholdingStrategyEnergy;
    thresholdEnergy = 1.0f;
    thresholdDim = 0;
    whitening = false;
    dualDim = 0;
}
    
void StandardPrePcaVlatCluster::setThresholdingStrategy(int s){
    if(s == StandardPrePcaVlatCluster::thresholdingStrategyDim ||
       s == StandardPrePcaVlatCluster::thresholdingStrategyEnergy){
        thresholdingStrategy = s;
    }
    else
        retinThrowException("Invalid thresholding strategy");
}

void StandardPrePcaVlatCluster::setEnergyThreshold(float e){
    if(e >= 0.0f && e <= 1.0f){
        thresholdEnergy = e;
    }
}

void StandardPrePcaVlatCluster::setDimThreshold(size_t d){
    if(d <= mainOrder);
        thresholdDim = d;
}

void StandardPrePcaVlatCluster::setWhitening(bool w){
    whitening = w;
}

void StandardPrePcaVlatCluster::computeDimDual(){
    
    if (!getMeanTensor(1))
        retinThrowException("Mean Tensor 1 missing");
    if (!getMeanTensor(2))
        retinThrowException("Mean Tensor 2 missing");
    
    if(thresholdingStrategy == StandardPrePcaVlatCluster::thresholdingStrategyDim) {
        dualDim = std::min(L.size(),thresholdDim);
    }
    else if(thresholdingStrategy == StandardPrePcaVlatCluster::thresholdingStrategyEnergy) {
        float sumTotal = 0.0f;
        float sum = 0.0f;
        size_t i;
        
        for(i = 0 ; i < L.size() ; i++){
            sumTotal += L[i];
        }
        
        i = 0;
        sum = 0;        
        while(sum < sumTotal*thresholdEnergy && L[i] > 1E-7 && L[i] > L[0]*1E-5 && i < L.size())
            sum += L[i++];
        
        dualDim = i;
    }
    else
        retinThrowException("Invalid thresholding strategy");

    sqrtinvL.resize(dualDim);        
    for(size_t i = 0 ; i < dualDim ; i++) {
        sqrtinvL[i] = sqrt(1.0f/L[i]);
    }    

    if (!vector_isfinite_float(&sqrtinvL[0],dualDim)) {
        retinThrowException("Not finite");
    }
    
}

void StandardPrePcaVlatCluster::setMainOrder(size_t dim){
    StandardVlatCluster::setMainOrder(dim);
    thresholdDim = dim;
}

void StandardPrePcaVlatCluster::setMeanTensor(size_t order,const float* meanTensor,size_t meanTensorDim){
    StandardVlatCluster::setMeanTensor(order, meanTensor, meanTensorDim);
    if(order == 2){
        size_t sqrtMeanTensorDim = (size_t) sqrt(meanTensorDim);
        if(sqrtMeanTensorDim*sqrtMeanTensorDim != meanTensorDim)
            retinThrowException("Not square matrix");
        U.resize(meanTensorDim);
        L.resize(sqrtMeanTensorDim);
        memcpy(&U[0], meanTensor, meanTensorDim * sizeof(float));
        matrix_eigSym_float (&U[0], &L[0], tensorDims[0]);
        matrix_sortEig_float (&U[0], &L[0], tensorDims[0]);
    }
}
    
void StandardPrePcaVlatCluster::init(){
    computeDimDual();
    size_t dim = 0;
    if(mainOrder == 1)
        dim = dualDim;
    else if(mainOrder == 2){
        dim = dualDim*dualDim;
    }
    vlat.resize(dim);
    memset(&vlat[0],0,dim*sizeof(float));
}

void StandardPrePcaVlatCluster::add(const float* feature,size_t featureDim){
    size_t meanDim = getTensorDim(1);
    if (featureDim != meanDim)
        retinThrowException2("Invalid feature dim %d != %d",featureDim,meanDim);

    vector<float> featureCentred(meanDim);
    vector<float> featureDual(dualDim);
    
    const float* mean = getMeanTensor(1);
    
    vector_linear_float(&featureCentred[0], feature, -1, mean, meanDim);
    matrix_CpAtB_float(&featureDual[0], &U[0], &featureCentred[0], dualDim, meanDim, 1);
    
    if(whitening)
        vector_mul_float (&featureDual[0], &sqrtinvL[0], dualDim);

    if (mainOrder == 1) {
        vector_add_float (&vlat[0], &featureDual[0], dualDim);
    }
    else if (mainOrder == 2) {
        matrix_Cpaat_float(&vlat[0], &featureDual[0], dualDim);
        
        if(whitening){
            for(size_t i = 0; i < dualDim ; i++)
                vlat[i*dualDim + i] -= 1.0f;
        }
        else{
            for(size_t i = 0; i < dualDim ; i++)
                vlat[i*dualDim + i] -= L[i];
        }
    }
}

size_t StandardPrePcaVlatCluster::getVlat(float* buf,const std::string& format) const{
    /*if (dualDim == 0){
        retinThrowException("Uncomputed dual dim");
    }*/
        
    if (mainOrder == 1) {
        if (buf)
            memcpy(buf,&vlat[0],dualDim*sizeof(float));
        return dualDim;
    }
    else if (mainOrder == 2) {
        if (format == "full") {
            if (buf)
                memcpy(buf,&vlat[0],dualDim*dualDim*sizeof(float));
            return dualDim*dualDim;
        }
        else if (format == "LD") {
            if (buf) {
                for (size_t i=0;i<dualDim;i++) {
                    for (size_t j=i;j<dualDim;j++)
                        *buf++ = vlat[i+j*dualDim];
                }
            }
            return (dualDim*(dualDim+1))/2;
        }
        else if (format == "L") {
            if (buf) {
                for (size_t i=0;i<dualDim;i++) {
                    for (size_t j=i+1;j<dualDim;j++)
                        *buf++ = vlat[i+j*dualDim];
                }
            }
            return (dualDim*(dualDim-1))/2;
        }
        else {
            retinThrowException1("Unsupported format %s", format.c_str());
        }
    }
    else {
        retinThrowException1("Unsupported order %d", mainOrder);
    }
}

StandardPrePcaVlatBuilder::StandardPrePcaVlatBuilder(size_t order,size_t dictSize,size_t featureDim): VlatBuilder<StandardPrePcaVlatCluster>(order,dictSize,featureDim){
}

StandardPrePcaVlatBuilder::~StandardPrePcaVlatBuilder(){
    
}

void StandardPrePcaVlatBuilder::computeDimDual(){
    for (size_t c=0;c<clusters.size();c++) {
        clusters[c].computeDimDual();
    }
}

void StandardPrePcaVlatBuilder::setThresholdingStrategy(int s){
    for (size_t c=0;c<clusters.size();c++) {
        clusters[c].setThresholdingStrategy(s);
    }
}

void StandardPrePcaVlatBuilder::setEnergyThreshold(float e){
    for (size_t c=0;c<clusters.size();c++) {
        clusters[c].setEnergyThreshold(e);
    }
}

void StandardPrePcaVlatBuilder::setDimThreshold(size_t d){
    for (size_t c=0;c<clusters.size();c++) {
        clusters[c].setDimThreshold(d);
    }
}

void StandardPrePcaVlatBuilder::setWhitening(bool w){
    for (size_t c=0;c<clusters.size();c++) {
        clusters[c].setWhitening(w);
    }
}

size_t    StandardPrePcaVlatBuilder::getVlatDim(const std::string& format) const {
    size_t dim = 0;
    for (size_t c=0;c<clusters.size();c++) {
        dim += clusters[c].getVlat(NULL,format);
    }
    return dim;
}

void    StandardPrePcaVlatBuilder::getVlat(float* buffer,const std::string& format) const {
    size_t dim = 0;
    for (size_t c=0;c<clusters.size();c++) {
        dim += clusters[c].getVlat(buffer+dim,format);
    }
}


}