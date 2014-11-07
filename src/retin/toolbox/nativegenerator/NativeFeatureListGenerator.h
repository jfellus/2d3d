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
