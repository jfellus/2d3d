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

#include "CwPooledVlatBuilder.h"

#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"

using namespace std;

namespace vlat {

/*CwPooledVlatCluster::CwPooledVlatCluster()
{
    
}

void    CwPooledVlatCluster::setCount(size_t count) {
    tensors.resize(count);
}

void    CwPooledVlatCluster::init() {
    highestNorm = 0;
}

void    CwPooledVlatCluster::add(const float* feature,size_t dim)
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
    vector<float> tensor(tensorDim);
    if (mainOrder == 1) {
        tensor = temp;
    }
    else if (mainOrder == 2) {
        matrix_Cpaat_float(&tensor[0], &temp[0], meanDim);
        const float* meanTensor = getMeanTensor(mainOrder);
        if (meanTensor)
            vector_sub_float(&tensor[0], meanTensor, tensorDim);
    }

    size_t minIdx = 0;
    for (size_t i=1;i<tensors.size();i++) {
        if (tensors[i].norm <  tensors[minIdx].norm)
            minIdx = i;
    }
    float norm = vector_n2_float (&tensor[0],tensorDim);
    if (norm > tensors[minIdx].norm) {
        tensors[minIdx].norm = norm;
        tensors[minIdx].values = tensor;
    }
    
}

size_t  CwPooledVlatCluster::getVlat(float* buf,const std::string& format) const {
    
    // Pool selected values
    size_t count = 0;
    size_t dim = getTensorDim(mainOrder);
    vector<float> vlat(dim);
    for (size_t i=0;i<tensors.size();i++) {
        if (tensors[i].norm <= 0)
            continue;
        //vector_pow_float((float*)&tensors[i].values[0],0.2,dim);
        vector_add_float(&vlat[0],&tensors[i].values[0],dim);
        count ++;
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
}*/

CwPooledVlatBuilder::CwPooledVlatBuilder(size_t order,size_t dictSize,size_t featureDim,size_t count)
             : VlatBuilder<StandardVlatCluster>(order,dictSize,featureDim)
{
}
CwPooledVlatBuilder::~CwPooledVlatBuilder()
{
}


void CwPooledVlatBuilder::process(const float* bag, size_t bagSize, size_t bagcenterDim) 
{
        if (bagcenterDim != centerDim)
            retinThrowException2("Wrong bag feature dim %d != %d", bagcenterDim, centerDim);

        init();
        
        float highestNorm = 0;
        vector<float> highestNorms(clusters.size());
        vector<size_t> clusterIdx(bagSize);
        vector<float> norms(bagSize);
        vector<float> temp(centerDim);
        for (size_t i = 0; i < bagSize; i++) {
            const float* data = bag + i*centerDim;
                        
            size_t idx = matrix_argmin_l2_float(data,&centers[0],centerDim,dictSize);
            clusterIdx[i] = idx;
            
            float norm = clusters[idx].getTensorNorm(data,centerDim);
            norms[i] = norm;
            if (norm > highestNorm)
                highestNorm = norm;
            if (norm > highestNorms[idx])
                highestNorms[idx] = norm;
        }
        
        size_t count = 0;
        
        for (size_t i = 0; i < bagSize; i++) {
            size_t idx = clusterIdx[i];
            if (norms[i] >= 0.45*highestNorms[idx]) {
                const float* data = bag + i*centerDim;
                add(idx,data,centerDim);
            }
            else count ++;
        }
        
        if (count > 0)
         cout << count << "/" << bagSize << " discared" << endl;

        finit();
}


size_t    CwPooledVlatBuilder::getVlatDim(const std::string& format) const {
    size_t dim = 0;
    for (size_t c=0;c<clusters.size();c++) {
        dim += clusters[c].getVlat(NULL,format);
    }
    return dim;
}

void    CwPooledVlatBuilder::getVlat(float* buffer,const std::string& format) const {
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

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPooledVlatBuilder_run
  (JNIEnv * env, jobject thisObj, jobject inputObj, jint degre, jstring formatObj)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureListRef> tensors; je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");
    string format = je.createString(formatObj);
    int count = 1000;

    CwPooledVlatBuilder vlat(degre,centers->size(),centers->dim(),count);
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

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPooledVlatBuilder_runImage
  (JNIEnv * env, jobject thisObj, jstring imageFileObj, jint degre, jstring formatObj, jstring featuretypeObj, jobject featureParamsObj)
{
    RETIN_JAVA_METHOD_BEGIN
/*
    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureListRef> tensors; je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");
    string format = je.createString(formatObj);
    int count = 3;
    
    string featuretype = je.createString(featuretypeObj);
    ParametersRef featureParams = je.createDocument<Parameters> (featureParamsObj);

    CwPooledVlatBuilder vlat(degre,centers->size(),centers->dim(),count);
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
