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

#include "EwPooledVlatBuilder.h"

#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"

#define CAP_VALUE 0

using namespace std;

namespace vlat {

EwPooledVlatCluster::EwPooledVlatCluster() :
    valuesCount(3)
{
    
}
    
void    EwPooledVlatCluster::init() {
    size_t dim = getTensorDim(mainOrder);
    maxValues.resize(dim * valuesCount);
    vector_scpy_float (&maxValues[0],-CAP_VALUE,maxValues.size());
}

void    EwPooledVlatCluster::poolValues(const float* values)
{
    size_t dim = getTensorDim(mainOrder);
    for (size_t i=0;i<dim;i++) {
        float v = sign*values[i];
        float* maxs = &maxValues[i*valuesCount];
        if (maxs[0] < v) {
            maxs[0] = v;
            for (size_t j=1;j<valuesCount;j++) {
                if (maxs[j-1] < maxs[j])
                    break;
                std::swap(maxs[j-1],maxs[j]);
            }
        }
    }
    /*cout << "sign=" << sign << " => ";
    for (size_t j=0;j<valuesCount;j++)
        cout << maxValues[j] << " ";
    cout << endl;*/
}

void    EwPooledVlatCluster::add(const float* feature,size_t dim)
{
    size_t meanDim = getTensorDim(1);
    if (dim != meanDim)
        retinThrowException2("Invalid feature dim %d != %d",dim,meanDim);

    vector<float> temp(meanDim);
    const float* mean = getMeanTensor(1);
    if (mean)
        vector_linear_float(&temp[0], feature, -1, mean, meanDim);
    else
        memcpy(&temp[0],feature,meanDim*sizeof(float));

    size_t tensorDim = getTensorDim(mainOrder);
    vector<float> values(tensorDim);
    if (mainOrder == 1) {
        values = temp;
    }
    else if (mainOrder == 2) {
        matrix_Cpaat_float(&values[0], &temp[0], meanDim);
        const float* tensor = getMeanTensor(mainOrder);
        if (tensor)
            vector_sub_float(&values[0], tensor, tensorDim);
    }
    
    poolValues(&values[0]);
}

size_t  EwPooledVlatCluster::getVlat(float* buf,const std::string& format) const {
    
    // Pool selected values
    size_t dim = getTensorDim(mainOrder);
    vector<float> vlat(dim);
    for (size_t i=0;i<dim;i++) {
        double sum = 0;
        size_t count = 0;
        const float* maxs = &maxValues[i*valuesCount];
        for (size_t j=0;j<valuesCount;j++) {
            if (maxs[j] > -CAP_VALUE) {
                sum += maxs[j];
                count ++;
            }
        }
        if (count > 0) {
            vlat[i] = powf(sum/count,0.5);
        }
    }
    
    size_t featureDim = getTensorDim(1);
    if (mainOrder == 1) {
        if (buf)
            memcpy(buf,&vlat[0],featureDim*sizeof(float));
        return featureDim;
    }
    else if (mainOrder == 2) {
        if (format == "full") {
            if (buf)
                memcpy(buf,&vlat[0],featureDim*featureDim*sizeof(float));
            return featureDim*featureDim;
        }
        else if (format == "LD") {
            if (buf) {
                for (size_t i=0;i<featureDim;i++) {
                    for (size_t j=i;j<featureDim;j++)
                        *buf++ = vlat[i+j*featureDim];
                }
            }
            return (featureDim*(featureDim+1))/2;
        }
        else if (format == "L") {
            if (buf) {
                for (size_t i=0;i<featureDim;i++) {
                    for (size_t j=i+1;j<featureDim;j++)
                        *buf++ = vlat[i+j*featureDim];
                }
            }
            return (featureDim*(featureDim-1))/2;
        }
        else {
            retinThrowException1("Unsupported format %s", format.c_str());
        }
    }
    else {
        retinThrowException1("Unsupported order %d", mainOrder);
    }
}

EwPooledVlatBuilder::EwPooledVlatBuilder(size_t order,size_t dictSize,size_t featureDim,float sign)
             : VlatBuilder<EwPooledVlatCluster>(order,dictSize,featureDim)
{
    for (size_t c=0;c<clusters.size();c++) {
        clusters[c].setSign(sign);
    }
}
EwPooledVlatBuilder::~EwPooledVlatBuilder()
{
}

size_t    EwPooledVlatBuilder::getVlatDim(const std::string& format) const {
    size_t dim = 0;
    for (size_t c=0;c<clusters.size();c++) {
        dim += clusters[c].getVlat(NULL,format);
    }
    return dim;
}

void    EwPooledVlatBuilder::getVlat(float* buffer,const std::string& format) const {
    size_t dim = 0;
    for (size_t c=0;c<clusters.size();c++) {
        dim += clusters[c].getVlat(buffer+dim,format);
    }
}

}


////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/Parameters.h"
#include "retin/toolbox/document/ImageFile.h"
/*
#include "retin/feature/vlfeat/DenseSiftExtractor.h"
#include "retin/feature/dfeat/DaveFeature.h"
#include "retin/feature/spof/Spof.h"
*/
using namespace retin;
using namespace boost;
using namespace vlat;

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_EwPooledVlatBuilder_run
  (JNIEnv * env, jobject thisObj, jobject inputObj, jint degre, jstring formatObj)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureListRef> tensors; je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");
    string format = je.createString(formatObj);
    jfloat sign = je.getField<jfloat>(thisObj,"sign");

    EwPooledVlatBuilder vlat(degre,centers->size(),centers->dim(),sign);
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int d=0;d<degre;d++) {
        FeatureListRef tensor = tensors[d];
        vlat.setMeanTensors(d+1,tensor->data(),tensor->size(),tensor->dim());
    }

    FeatureListRef input = je.createDocument<FeatureList > (inputObj);
    vlat.process(input->data(),input->size(),input->dim());

    FeatureRef outputRef = boost::make_shared<Feature>(vlat.getVlatDim(format));
    vlat.getVlat(outputRef->data(),format);

    return je.createObject (outputRef);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_EwPooledVlatBuilder_runImage
  (JNIEnv * env, jobject thisObj, jstring imageFileObj, jint degre, jstring formatObj, jstring featuretypeObj, jobject featureParamsObj)
{
    RETIN_JAVA_METHOD_BEGIN
/*
    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureListRef> tensors; je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");
    string format = je.createString(formatObj);
    jfloat sign = je.getField<jfloat>(thisObj,"sign");
    
    string featuretype = je.createString(featuretypeObj);
    ParametersRef featureParams = je.createDocument<Parameters> (featureParamsObj);

    EwPooledVlatBuilder vlat(degre,centers->size(),centers->dim(),sign);
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int d=0;d<degre;d++) {
        FeatureListRef tensor = tensors[d];
        vlat.setMeanTensors(d+1,tensor->data(),tensor->size(),tensor->dim());
    }
    
    string imageFile = je.createString(imageFileObj);
    
    FeatureListRef features;
    
    if(featuretype == "dsift")
    {
        size_t step = featureParams->getAs<size_t>("spacing");
        std::vector<size_t> scales;
        
        std::vector<std::string> stringScales = (*featureParams)("scales").getNames();
        
        for(unsigned int i = 0 ; i < stringScales.size() ; i++){
            scales.push_back(boost::lexical_cast<size_t> (stringScales[i]));
        }
        
        float minX, minY, maxX, maxY;
        
        minX = featureParams->getAs<float>("minX");
        minY = featureParams->getAs<float>("minY");
        maxX = featureParams->getAs<float>("maxX");
        maxY = featureParams->getAs<float>("maxY");
        
         if(featureParams->get("useFlatWindow").compare("yes")==0)
	     	features = denseSiftExtraction (step, scales, minX, minY, maxX, maxY, imageFile.c_str(),"useFlatWindow=yes");
	     else
	     	features = denseSiftExtraction (step, scales, minX, minY, maxX, maxY, imageFile.c_str());
    }
    else if(featuretype == "dfeat") {
		size_t step = featureParams->getAs<size_t>("spacing");
        std::vector<size_t> scales;
        
        std::vector<std::string> stringScales = (*featureParams)("scales").getNames();
        
        for(unsigned int i = 0 ; i < stringScales.size() ; i++){
            scales.push_back(boost::lexical_cast<size_t> (stringScales[i]));
        }
        
        float minX, minY, maxX, maxY;
        
        minX = featureParams->getAs<float>("minX");
        minY = featureParams->getAs<float>("minY");
        maxX = featureParams->getAs<float>("maxX");
        maxY = featureParams->getAs<float>("maxY");
        string feature = featureParams->get("type");
        
        if(feature == "sift")
			features = dfeat::extractDaveDSift (step, scales, minX, minY, maxX, maxY, imageFile.c_str());
		else if(feature == "hog")
			features = dfeat::extractDaveHog (step, scales, minX, minY, maxX, maxY, imageFile.c_str());
		else if(feature == "fastsift")
			features = dfeat::extractDaveFastSift (step, scales, minX, minY, maxX, maxY, imageFile.c_str());
	}
    else if(featuretype == "spof") {
		size_t step = featureParams->getAs<size_t>("spacing");
		size_t deg = featureParams->getAs<size_t>("degre");
        std::vector<size_t> scales;
        
        std::vector<std::string> stringScales = (*featureParams)("scales").getNames();
        
        for(unsigned int i = 0 ; i < stringScales.size() ; i++){
            scales.push_back(boost::lexical_cast<size_t> (stringScales[i]));
        }
        
        float minX, minY, maxX, maxY;
        
        minX = featureParams->getAs<float>("minX");
        minY = featureParams->getAs<float>("minY");
        maxX = featureParams->getAs<float>("maxX");
        maxY = featureParams->getAs<float>("maxY");
        
		features = spof::extractSpof(deg, step, scales, minX, minY, maxX, maxY, imageFile.c_str());
        retinThrowException("unsupported");
	}
            
    vlat.process(features->data(),features->size(),features->dim());

    FeatureRef outputRef = boost::make_shared<Feature>(vlat.getVlatDim(format));
    vlat.getVlat(outputRef->data(),format);

    return je.createObject (outputRef);
*/
    retinThrowException("unsupported");
    RETIN_JAVA_METHOD_END
    return NULL;
}

#endif
