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
#include "StandardPcaVlatBuilder.h"

#include <math.h>
#include <string.h>

#include "retin/toolbox/algebra/matrix_double.h"
#include "retin/toolbox/algebra/vector_double.h"
#include "retin/toolbox/algebra/matrix_float.h"

using namespace retin;
using namespace boost;
using namespace std;

namespace vlat {

StandardPcaVlatCluster::StandardPcaVlatCluster(FeatureRef meanPrim, MatrixRef proj, 
            MatrixRef var, size_t dimPrim, size_t dimDual, bool whitening, int degre):
                    dimPrim(dimPrim), dimDual(dimDual), meanPrim(meanPrim),
                    proj(proj), var(var), counter(0), whitening(whitening), degre(degre){
    
                featc = boost::make_shared<Matrix>(dimPrim,1);
                featDual = boost::make_shared<Matrix>(dimPrim,1);

                invsqrtvar = boost::make_shared<Matrix>(var->vectorSize(), 1);

                for(int i = 0 ; i < var->vectorSize() ; i++){
                    (*invsqrtvar)(i,0) = 1.0/sqrt((*var)(i,0));
                }
                if(degre == 1){
                    vlat = boost::make_shared<Matrix>(dimDual,1);
                    memset(vlat->data(),0,sizeof(double)*dimDual);
                }
                else if(degre == 2){
                    vlat = boost::make_shared<Matrix>(dimDual,dimDual);
                    memset(vlat->data(),0,sizeof(double)*dimDual*dimDual);
                }
            }
    
FeatureRef StandardPcaVlatCluster::getData(string & format)
{
    FeatureRef feat = boost::make_shared<Feature>(getDim(format));
    if(degre == 1){
        for(size_t i = 0 ; i < dimDual; i++)
                (*feat)(i) = (float)((*vlat)(i));
    }
    else if(degre == 2){
        if(format == "full"){
            for(size_t j = 0 ; j < dimDual ; j++)
            {
                for(size_t i = 0 ; i < dimDual; i++)
                    (*feat)(i+j*dimDual) = (float)((*vlat)(i,j));

                if(whitening)
                    (*feat)(j+j*dimDual) = (float)((*vlat)(j,j) - counter*1.0);
                else
                    (*feat)(j+j*dimDual) = (float)((*vlat)(j,j) - counter*(*var)(j,0));
            }
        }
        else if(format == "LD"){
            size_t ind = 0;
            for(size_t j = 0 ; j < dimDual ; j++)
            {
                if(whitening)
                    (*feat)(ind) = (float)((*vlat)(j,j) - counter*1.0);
                else
                    (*feat)(ind) = (float)((*vlat)(j,j) - counter*(*var)(j,0));
                ind++;
                for(size_t i = j+1 ; i < dimDual; i++)
                {
                    (*feat)(ind) = (float)((*vlat)(i,j));
                    ind++;
                }
            }
        }
        else if(format == "L"){
            size_t ind = 0;
            for(size_t j = 0 ; j < dimDual ; j++)
            {
                for(size_t i = j+1 ; i < dimDual; i++)
                {
                    (*feat)(ind) = (float)((*vlat)(i,j));
                    ind++;
                }
            }
        }
    }

    return feat;
}

size_t StandardPcaVlatCluster::getDim(string & format){
    if(degre == 1){
        return dimDual;
    }
    else if(degre == 2){
        if(format == "full")
            return dimDual*dimDual;
        else if(format == "LD")
            return dimDual*(dimDual+1)/2;
        else if(format == "L")
            return dimDual*(dimDual-1)/2;
        }
    return 0;
}

void StandardPcaVlatCluster::add(FeatureRef feat){
    add(feat->data(), feat->dim());    
}

void StandardPcaVlatCluster::add(const float * f, size_t dim){
    if(dim != dimPrim)
        retinThrowException("StandardPcaVlatCluster::add => dim != dimPrim");
    
    counter++;
    
    for(size_t i = 0 ; i<dimPrim ; i++)
    {
        (*featc)(i,0) = f[i] - (*meanPrim)(i);
        
    }

    for (size_t k = 0; k < dimPrim; k++) {
        (featDual->data())[k] = vector_ps_double(featc->data(), proj->data()+k*dimPrim, dimPrim);
    }

    if(whitening)
        vector_mul_double(featDual->data(), invsqrtvar->data(), dimPrim);

    if(degre == 1)
        vector_add_double(vlat->data(), featDual->data(), dimDual);
    else
        matrix_Cpaat_double(vlat->data(), featDual->data(), dimDual);
}


void StandardPcaVlatBuilder::process(FeatureListRef features){
    for (int i = 0; i < features->size(); i++) {
        const float* data = (*features)[i];
        
        size_t cMin = matrix_argmin_l2_float(data,center->data(),center->dim(),center->size());
        
        clusters[cMin]->add(data, features->dim());
    }
}

size_t StandardPcaVlatBuilder::getDim(string & format){
    size_t dim = 0;
    for(size_t i = 0 ; i < clusters.size(); i++)
        dim +=  clusters[i]->getDim(format);
    return dim;
}

FeatureRef StandardPcaVlatBuilder::getVlat(string & format){
    FeatureRef vlat;
    size_t dim = getDim(format);
    vlat = boost::make_shared<Feature>(dim);
    float * data = vlat->data();
    
    for(size_t i = 0 ; i < clusters.size(); i++){
        FeatureRef subVlat = clusters[i]->getData(format);
        memcpy(data, subVlat->data(), subVlat->dim()*sizeof(float));
        data += subVlat->dim();
        if((size_t)(data - vlat->data()) > dim)
            retinThrowException("StandardPcaVlatCluster::getVlat => data - vlat->data() > dim");
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

#include "retin/toolbox/nativegenerator/NativeDocumentGenerator.h"


using namespace vlat;

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_StandardPcaVlatBuilder_run
  (JNIEnv * env, jobject thisObj, jobject input, jint degre, jstring formatObj)
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

    jboolean whitening = je.getField<jboolean>(thisObj,"whitening");
            
    string format = je.createString(formatObj);
    
    StandardPcaVlatBuilder vlat(centers, tensors, vectors, values, dims, 
            whitening, centers->size(), degre);

    FeatureListRef features = je.createDocument<FeatureList> (input);;
    
    vlat.process(features);
    
    return je.createObject (vlat.getVlat(format));

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_StandardPcaVlatBuilder_runNativeGenerator
  (JNIEnv *env, jobject thisObject, jobject nativePointerObj, jstring keyObj, jint degre, jstring formatObj){
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

    jboolean whitening = je.getField<jboolean>(thisObject,"whitening");
            
    string format = je.createString(formatObj);
    
    
    NativePointerRef instance = je.createDocument<NativePointer>(nativePointerObj);
    NativeDocumentGenerator *ngd = instance->getPointerAs<NativeDocumentGenerator>();

    FeatureListRef features = boost::static_pointer_cast<FeatureList>(ngd->computeDocument(key));

    StandardPcaVlatBuilder vlat(centers, tensors, vectors, values, dims, 
            whitening, centers->size(), degre);

    vlat.process(features);
    
    return je.createObject (vlat.getVlat(format));
    
    RETIN_JAVA_METHOD_END
    return NULL;
}

#endif
