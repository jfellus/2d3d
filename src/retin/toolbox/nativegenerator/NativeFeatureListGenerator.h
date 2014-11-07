/* 
 * File:   NativeFeatureListGenerator.h
 * Author: romanegr
 *
 * Created on 14 juin 2013, 10:52
 */

#include "NativeDocumentGenerator.h"

#ifndef __NATIVEFEATURELISTGENERATOR_H__
#define	__NATIVEFEATURELISTGENERATOR_H__

#include "retin/toolbox/document/list/FeatureList.h"


namespace retin
{

class NativeFeatureListGenerator : public NativeDocumentGenerator {
protected:
    bool pcaEn;
    bool whitening;
    //unsigned int dim;
    size_t dim;
    bool powerEn;
    float power;
    bool l2En;
    
    FloatMatrixRef mean;
    FloatMatrixRef invsqrteigValues;
    FloatMatrixRef eigValues;
    FloatMatrixRef eigVectors;
    
public:
    NativeFeatureListGenerator() : NativeDocumentGenerator(), pcaEn(false), whitening(false), dim(0), powerEn(false), power(0.5), l2En(false) {

    }
    
    virtual ~NativeFeatureListGenerator() {}
    
    virtual void initParams(ParametersRef params);
    
    virtual FeatureListRef computeFeatureListFile(std::string addressFile){
        retinThrowException("NativeFeatureListGenerator : computeFeatureListFile(std::string addressFile) : No implemented");
        return boost::make_shared<FeatureList>();
    }
    
    virtual DocumentRef computeFile(std::string addressFile);
    
    virtual size_t getDim(ParametersRef params){
        if(params->get("pca","no") == "yes"){
            return params->getSubParams("pca")->getAs<unsigned int>("dim");
        }
        return 0;
    }
    
    virtual size_t getDim(){
        if(pcaEn)
            return dim;
        return 0;
    }
    
    virtual void setMean(FloatMatrixRef _mean){
        mean = _mean;
    }
    
    virtual void setEigValues(FloatMatrixRef _eigValues){
        eigValues = _eigValues;
    }
    
    virtual void setEigVectors(FloatMatrixRef _eigVectors){
        eigVectors = _eigVectors;
    }
    
};

typedef boost::shared_ptr<NativeDocumentGenerator> NativeDocumentGeneratorRef;

}


#endif	/* __NATIVEFEATURELISTGENERATOR_H__ */

