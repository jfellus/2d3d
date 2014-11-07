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

#include "ProjectedVlatBuilder.h"

#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"

using namespace std;

namespace vlat {

void    ProjectedVlatCluster::init() {
    size_t dim = getTensorDim(mainOrder);
    pvlat.resize(dim);
    memset(&pvlat[0],0,dim*sizeof(float));
}

void    ProjectedVlatCluster::add(const float* feature, size_t dim)
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

    matrix_Cpaat_float(&pvlat[0], &temp[0], meanDim);
}

float    ProjectedVlatCluster::getTensorNorm(const float* feature,size_t dim) const
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

    return vector_n2_float(&temp[0],meanDim);
}

size_t  ProjectedVlatCluster::getVlat(float* buf) const {
    
    size_t featureDim = getTensorDim(1);
    size_t tensorDim = getTensorDim(mainOrder);
    const float* tensor = getMeanTensor(mainOrder);
    vector<float> invMeanTensor(tensorDim);
    if (tensor){
        vector<float> temp(tensorDim);
        memcpy(&temp[0], tensor, tensorDim*sizeof(float));
        matrix_inv_float (&invMeanTensor[0], &temp[0], featureDim);
    }
        
    if (buf){
        memset(buf, 0 ,featureDim*featureDim*sizeof(float));
        matrix_CpAB_float (buf, &pvlat[0], &invMeanTensor[0], featureDim, featureDim, featureDim);
    }
    
    return featureDim*featureDim;
}

ProjectedVlatBuilder::ProjectedVlatBuilder(size_t order,size_t dictSize,size_t featureDim)
             : VlatBuilder<ProjectedVlatCluster>(order,dictSize,featureDim)
{
}

ProjectedVlatBuilder::~ProjectedVlatBuilder()
{
}

size_t    ProjectedVlatBuilder::getVlatDim() const {
    size_t dim = 0;
    for (size_t c=0;c<clusters.size();c++) {
        dim += clusters[c].getVlat(NULL);
    }
    return dim;
}

void    ProjectedVlatBuilder::getVlat(float* buffer) const {
    size_t dim = 0;
    for (size_t c=0;c<clusters.size();c++) {
        if(buffer)
                dim += clusters[c].getVlat(buffer+dim);
    }
}

}


////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/Parameters.h"
#include "retin/toolbox/document/basic/NativePointer.h"

#include "retin/toolbox/nativegenerator/NativeDocumentGenerator.h"

using namespace retin;
using namespace boost;
using namespace vlat;

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_ProjectedVlatBuilder_run
  (JNIEnv * env, jobject thisObj, jobject inputObj)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureListRef> tensors; je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");

    ProjectedVlatBuilder vlat(2,centers->size(),centers->dim());
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int d=0;d<2;d++) {
        FeatureListRef tensor = tensors[d];
        vlat.setMeanTensors(d+1,tensor->data(),tensor->size(),tensor->dim());
    }

    FeatureListRef input = je.createDocument<FeatureList > (inputObj);
    vlat.process(input->data(),input->size(),input->dim());

    FeatureRef outputRef = boost::make_shared<Feature>(vlat.getVlatDim());
    vlat.getVlat(outputRef->data());

    return je.createObject (outputRef);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_ProjectedVlatBuilder_runNativeGenerator
  (JNIEnv *env, jobject thisObject, jobject nativePointerObj, jstring keyObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    string key = je.createString(keyObj);
    
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObject,"centers");

    vector<FeatureListRef> tensors; 
    je.getDocumentField<FeatureList> (tensors,thisObject,"tensors");
    
    NativePointerRef instance = je.createDocument<NativePointer>(nativePointerObj);
    NativeDocumentGenerator *ngd = instance->getPointerAs<NativeDocumentGenerator>();
    
    FeatureListRef features = boost::static_pointer_cast<FeatureList>(ngd->computeDocument(key));
    
    ProjectedVlatBuilder vlat(2 ,centers->size(),centers->dim());
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int d=0;d<2;d++) {
        FeatureListRef tensor = tensors[d];
        vlat.setMeanTensors(d+1, tensor->data(), tensor->size(), tensor->dim());
    }
    
    vlat.process(features->data(),features->size(),features->dim());

    FeatureRef outputRef = boost::make_shared<Feature>(vlat.getVlatDim());
    vlat.getVlat(outputRef->data());

    return je.createObject (outputRef);
    
    RETIN_JAVA_METHOD_END
    return NULL;
}

#endif
