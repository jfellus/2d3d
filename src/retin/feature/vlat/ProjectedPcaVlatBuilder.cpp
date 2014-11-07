/*
Copyright © CNRS 2012. 
Authors: Philippe-Henri Gosselin, David Picard, Romain Négrel
Contact: philippe-henri.gosselin@ensea.fr

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
#include "ProjectedPcaVlatBuilder.h"

#include <math.h>
#include <string.h>

#include "../../toolbox/algebra/matrix_double.h"
#include "../../toolbox/algebra/vector_double.h"
#include "../../toolbox/algebra/matrix_float.h"

using namespace retin;
using namespace boost;
using namespace std;

namespace vlat {

ProjectedPcaVlatCluster::ProjectedPcaVlatCluster(FeatureRef meanPrim, MatrixRef proj, 
            MatrixRef var, size_t dimPrim, size_t dimDual):
                    dimPrim(dimPrim), dimDual(dimDual), meanPrim(meanPrim),
                    proj(proj), var(var), counter(0){
    
                featc = boost::make_shared<Matrix>(dimPrim,1);
                featDual = boost::make_shared<Matrix>(dimPrim,1);
                meanDual = boost::make_shared<Matrix>(dimPrim,1);
                tensor = boost::make_shared<Matrix>(dimDual,dimDual);

                invvar = boost::make_shared<Matrix>(var->vectorSize(), 1);

                for(int i = 0 ; i < var->vectorSize() ; i++){
                    (*invvar)(i,0) = 1.0/(*var)(i,0);
                }
                
                memset(tensor->data(),0,sizeof(double)*dimDual*dimDual);
            }
    
FeatureRef ProjectedPcaVlatCluster::getData(string centered)
{
    FeatureRef subVlat = boost::make_shared<Feature>(getDim());
    
    if(centered == "yes"){
        /*retin::MatrixRef finalTensor = boost::make_shared<Matrix>(dimDual,dimDual);
        retin::MatrixRef meanTensor = boost::make_shared<Matrix>(dimDual,dimDual);

        memcpy(finalTensor->data(),tensor->data(),sizeof(double)*dimDual*dimDual);

        matrix_Cpaat_double(meanTensor->data(), meanDual->data(), dimDual);
    
        vector_sub_double(finalTensor->data(),meanTensor->data(),dimDual*dimDual);
        
        for(size_t j = 0 ; j < dimDual ; j++)
        {
            for(size_t i = 0 ; i < dimDual; i++)
                (*subVlat)(i+j*dimDual) = (float)((*finalTensor)(i,j)*(*invvar)(j,0));
        }*/
        
    }
    else {
        for(size_t j = 0 ; j < dimDual ; j++)
        {
            for(size_t i = 0 ; i < dimDual; i++)
                (*subVlat)(i+j*dimDual) = (float)((*tensor)(i,j)*(*invvar)(j,0));
        }
    
    }

    return subVlat;
}

size_t ProjectedPcaVlatCluster::getDim(){
        return dimDual*dimDual;
}

void ProjectedPcaVlatCluster::add(FeatureRef feat){
    add(feat->data(), feat->dim());    
}

void ProjectedPcaVlatCluster::add(const float * f, size_t dim){
    if(dim != dimPrim)
        retinThrowException("ProjectedPcaVlatCluster::add => dim != dimPrim");
    
    /*counter++;
    
    for(size_t i = 0 ; i<dimPrim ; i++)
    {
        (*featc)(i,0) = f[i] - (*meanPrim)(i);
    }
    
    memset(featDual->data(), 0, sizeof(double)*featDual->vectorSize()*featDual->vectorCount());
    
    
    for (size_t k = 0; k < dimPrim; k++) {
        (featDual->data())[k] = vector_ps_double(featc->data(), proj->data()+k*dimPrim, dimPrim);
    }
    vector_add_double (meanDual->data(), featDual->data(), dimPrim);
    
    //matrix_CpAtB_double(featDual->data(), proj->data(), featc->data(), dimPrim, dimPrim, 1);
    
    
    matrix_Cpaat_double(tensor->data(), featDual->data(), dimDual);*/
}


void ProjectedPcaVlatBuilder::process(FeatureListRef features){
    //int test = 0;
    for (int i = 0; i < features->size(); i++) {
        const float* data = (*features)[i];
        size_t cMin = matrix_argmin_l2_float(data,center->data(),center->dim(),center->size());
        /*if(data[0]==0.0)
            test++;*/
        clusters[cMin]->add(data, features->dim());
    }
    //cout << test << "/" << features->size() << " (" << (test*100.0f)/features->size() << ")" << endl;
}

size_t ProjectedPcaVlatBuilder::getDim(){
    size_t dim = 0;
    for(size_t i = 0 ; i < clusters.size(); i++)
        dim +=  clusters[i]->getDim();
    return dim;
}

FeatureRef ProjectedPcaVlatBuilder::getVlat(string centered){
    FeatureRef vlat;
    size_t dim = getDim();
    vlat = boost::make_shared<Feature>(dim);
    float * data = vlat->data();
    
    for(size_t i = 0 ; i < clusters.size(); i++){
        FeatureRef subVlat = clusters[i]->getData(centered);
        memcpy(data, subVlat->data(), subVlat->dim()*sizeof(float));
        data += subVlat->dim();
        if((size_t)(data - vlat->data()) > dim)
            retinThrowException("ProjectedPcaVlatCluster::getVlat => data - vlat->data() > dim");
    }
    return vlat;
}


}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "../../toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/Parameters.h"
#include "retin/toolbox/document/basic/NativePointer.h"
#include "retin/toolbox/document/basic/Integer.h"
#include "retin/toolbox/document/basic/String.h"

#include "retin/toolbox/nativegenerator/NativeDocumentGenerator.h"


using namespace vlat;

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_ProjectedPcaVlatBuilder_run
  (JNIEnv * env, jobject thisObj, jobject input)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");

    vector<FeatureListRef> tensors; 
    je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");

    vector<MatrixRef> vectors;
    je.getDocumentField<Matrix> (vectors,thisObj,"vectors");

    vector<MatrixRef> values;
    je.getDocumentField<Matrix> (values,thisObj,"values");

    vector<IntegerRef> dims;
    je.getDocumentField<retin::Integer> (dims,thisObj,"dims");
            
    string centered;
    centered = je.getDocumentField<retin::String>(thisObj,"centered")->get();
    
    ProjectedPcaVlatBuilder vlat(centers, tensors, vectors, values, dims, centers->size());
    
    
    FeatureListRef features = je.createDocument<FeatureList> (input);;
    
    vlat.process(features);
    
    return je.createObject (vlat.getVlat(centered));

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_ProjectedPcaVlatBuilder_runNativeGenerator
  (JNIEnv *env, jobject thisObject, jobject nativePointerObj, jstring keyObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    string key = je.createString(keyObj);
    
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObject,"centers");

    vector<FeatureListRef> tensors; 
    je.getDocumentField<FeatureList> (tensors,thisObject,"tensors");

    vector<MatrixRef> vectors;
    je.getDocumentField<Matrix> (vectors,thisObject,"vectors");

    vector<MatrixRef> values;
    je.getDocumentField<Matrix> (values,thisObject,"values");

    vector<IntegerRef> dims;
    je.getDocumentField<retin::Integer> (dims,thisObject,"dims");
    
    string centered;
    centered = je.getDocumentField<retin::String>(thisObject,"centered")->get();
    
    NativePointerRef instance = je.createDocument<NativePointer>(nativePointerObj);
    NativeDocumentGenerator *ngd = instance->getPointerAs<NativeDocumentGenerator>();
    
    FeatureListRef features = boost::static_pointer_cast<FeatureList>(ngd->computeDocument(key));
    
    ProjectedPcaVlatBuilder vlat(centers, tensors, vectors, values, dims 
            , centers->size());
    
    vlat.process(features);
    
    return je.createObject (vlat.getVlat(centered));
    
    RETIN_JAVA_METHOD_END
    return NULL;
}

#endif
