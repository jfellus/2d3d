#include "SparseVlatBuilder.h"

#include <utility>
#include <algorithm>  

#include "retin/toolbox/algebra/matrix_float.h"

using namespace std;

namespace vlat {
    
    static bool myfunction (pair <size_t, float> i,pair <size_t, float> j) { return (i.second > j.second); }
    
    SparseVlatCluster::SparseVlatCluster(){
        sparsityStrategy = SparseVlatCluster::sparsityStrategyDim;
        sparsityOn = SparseVlatCluster::sparsityOnDiag;
        sparsityDim = 0;
        sparsityEnergy = 1.0;
    }
    
    void SparseVlatCluster::setSparsityOn(int o){
        if(SparseVlatCluster::sparsityOnDiag == o ||
                SparseVlatCluster::sparsityOnFull == o){
            sparsityOn = o;
        }
    }
    
    void SparseVlatCluster::setSparsityStrategy(int s){
        if(SparseVlatCluster::sparsityStrategyDim == s ||
                SparseVlatCluster::sparsityStrategyEnergy == s||
                SparseVlatCluster::sparsityMaxValues == s){
            sparsityStrategy = s;
        }
    }
    
    void SparseVlatCluster::setEnergySparsity(float e){
        if(e >= 0.0f && e <= 1.0f){
                sparsityEnergy = e;
        }
    }
    
    void SparseVlatCluster::setDimSparsity(size_t d){
        if(d <= mainOrder);
                sparsityDim = d;
    }
    
    void SparseVlatCluster::setMainOrder(size_t dim){
        StandardVlatCluster::setMainOrder(dim);
        if(sparsityDim == 0 || sparsityDim > dim)
                sparsityDim = dim;
    }
    
    void SparseVlatCluster::init(){
        StandardVlatCluster::init();
        size_t dim = getTensorDim(1);
        
        if(meanTensors[1] && L.size() == 0){
            U.resize(dim*dim);
            L.resize(dim);
            memcpy(&U[0], meanTensors[1], dim*dim*sizeof(float));
            matrix_eigSym_float (&U[0], &L[0], dim);
            matrix_sortEig_float (&U[0], &L[0], dim);
        }

        if(mainOrder == 1 || sparsityOn == SparseVlatCluster::sparsityOnDiag){
            varDual.reserve(dim);
            memset(&varDual[0],0,dim*sizeof(float));
        }
        else if(sparsityOn == SparseVlatCluster::sparsityOnFull){
            varDual.reserve(dim*dim);
            memset(&varDual[0],0,dim*dim*sizeof(float));
        }
        
        counter = 0;
    }
    
    void SparseVlatCluster::add(const float* feature, size_t featureDim){
        size_t meanDim = getTensorDim(1);
        if (featureDim != meanDim)
            retinThrowException2("Invalid feature dim %d != %d", featureDim, meanDim);

        vector<float> featureCentred(meanDim);
        const float* mean = getMeanTensor(1);
        if (mean)
            vector_linear_float(&featureCentred[0], feature, -1, mean, meanDim);
        else
            memcpy(&featureCentred[0],feature,meanDim*sizeof(float));
        
        vector<float> featureDual(meanDim);
        matrix_CpAtB_float(&featureDual[0], &U[0], &featureCentred[0], meanDim, meanDim, 1);
        
        if(mainOrder == 1 || sparsityOn == SparseVlatCluster::sparsityOnDiag){
            for(size_t i = 0 ; i < meanDim ; i++)
                varDual[i] += featureDual[i]*featureDual[i];
        }
        else if(sparsityOn == SparseVlatCluster::sparsityOnFull){
            vector<float> temp(meanDim*meanDim);
            matrix_Cpaat_float(&temp[0], &featureDual[0], meanDim);
            for(size_t i = 0 ; i < meanDim*meanDim ; i++)
                temp[i] *= temp[i];
            vector_add_float (&varDual[0], &featureDual[0], meanDim*meanDim);
        }
        
        vector<pair<size_t, float> > sortTable;
        
        if (mainOrder == 1) {
            if(sparsityOn == sparsityMaxValues){
                sortTable.resize(meanDim);
                for(size_t i = 0 ; i < meanDim ; i++){
                        sortTable[i].first = i;
                        sortTable[i].second = featureDual[i];
                }
                sort(sortTable.begin(), sortTable.end(), myfunction);
                for(size_t i = sparsityDim ; i < meanDim ; i++){
                        featureDual[sortTable[i].first] = 0;
                }
            }
            vector_add_float (&vlat[0], &featureDual[0], meanDim);
        }
        else if (mainOrder == 2) {
            if(sparsityOn == sparsityMaxValues){
                vector<float> temp(meanDim*meanDim);
                memset(&temp[0],0,meanDim*meanDim*sizeof(float));
                matrix_Cpaat_float(&temp[0], &featureDual[0], meanDim);
                sortTable.resize(meanDim*meanDim);
                for(size_t i = 0 ; i < meanDim*meanDim ; i++){
                        sortTable[i].first = i;
                        sortTable[i].second = temp[i];
                }
                sort(sortTable.begin(), sortTable.end(), myfunction);
                for(size_t i = sparsityDim ; i < meanDim*meanDim ; i++){
                        temp[sortTable[i].first] = 0;
                }
                vector_add_float (&vlat[0], &temp[0], meanDim*meanDim);
            }
            else
                matrix_Cpaat_float(&vlat[0], &featureDual[0], meanDim);
        }
        counter++;
    }
    
    
    
    size_t SparseVlatCluster::getVlat(float* buf,const std::string& format) const{
        size_t featureDim = getTensorDim(1);
        size_t vlatDim = getTensorDim(mainOrder);
        vector<float> sparseVlat(vlatDim);
        vector<int> sparseMask;
        
        if (buf){
            memcpy(&sparseVlat[0], &vlat[0], vlatDim*sizeof(float));
            vector<pair<size_t, float> > sortTable;
            if(mainOrder == 1 || sparsityOn == SparseVlatCluster::sparsityOnDiag){
                sortTable.reserve(featureDim);
                sparseMask.reserve(featureDim);
                for(size_t i = 0 ; i < featureDim ; i++){
                        sortTable[i].first = i;
                        sortTable[i].second = varDual[i];
                }
                sort(sortTable.begin(), sortTable.end(), myfunction);
            }
            else if(sparsityOn == SparseVlatCluster::sparsityOnFull){
                sortTable.reserve(featureDim*featureDim);
                sparseMask.reserve(featureDim*featureDim);
                for(size_t i = 0 ; i < featureDim*featureDim ; i++){
                        sortTable[i].first = i;
                        sortTable[i].second = varDual[i];
                }
                sort(sortTable.begin(), sortTable.end(), myfunction);
            }

            if(sparsityStrategy == SparseVlatCluster::sparsityStrategyDim){
                for(size_t i = 0 ; i < sparsityDim ; i++)
                    sparseMask[sortTable[i].first] = 1;
                for(size_t i = sparsityDim ; i < sortTable.size() ; i++)
                    sparseMask[sortTable[i].first] = 0;
            }
            else if(sparsityStrategy == SparseVlatCluster::sparsityStrategyEnergy){
                float sumVar = 0;
                for(size_t i = 0 ; i < sortTable.size() ; i++)
                    sumVar += varDual[i];
                size_t i;
                float sum;
                for(i=0, sum = 0; i < sortTable.size() && sum<sumVar*sparsityEnergy ; i++){
                    sparseMask[sortTable[i].first] = 1;
                    sum += sortTable[i].second;
                }
                for( ; i < sortTable.size() ; i++)
                    sparseMask[sortTable[i].first] = 0;
            }
        }
        
        if (mainOrder == 1) {
            if (buf){
                memcpy(&sparseVlat[0], &vlat[0], featureDim*sizeof(float));
                for(size_t i = 0 ; i < featureDim; i++){
                    if(sparseMask[i] == 0){
                        sparseVlat[i] = 0.0f;
                    }
                }
            
                memcpy(buf, &sparseVlat[0], featureDim*sizeof(float));
            }
            return featureDim;
        }
        else if (mainOrder == 2) {
            if (buf){
                if(sparsityOn == SparseVlatCluster::sparsityOnDiag){
                    memcpy(&sparseVlat[0], &vlat[0], vlatDim*sizeof(float));
                    for(size_t i = 0 ; i < featureDim; i++){
                        if(sparseMask[i] == 0){
                            for(size_t j = 0 ; j < featureDim ; j++){
                                sparseVlat[j+featureDim*i] = 0.0f;
                                sparseVlat[i+featureDim*j] = 0.0f;
                            }
                        }
                    }
                }
                else if(sparsityOn == SparseVlatCluster::sparsityOnFull){
                    memcpy(&sparseVlat[0], &vlat[0], vlatDim*sizeof(float));
                    for(size_t i = 0 ; i < vlatDim; i++){
                         if(sparseMask[i] == 0){
                             sparseVlat[i] = 0.0f;
                        }
                    }
                }
                for(size_t i = 0; i < featureDim ; i++)
                        sparseVlat[i*featureDim + i] -= counter*L[i];
            }
            
            if (format == "full") {
                if (buf)
                    memcpy(buf,&sparseVlat[0],featureDim*featureDim*sizeof(float));
                return featureDim*featureDim;
            }
            else if (format == "LD") {
                if (buf) {
                    for (size_t i=0;i<featureDim;i++) {
                        for (size_t j=i;j<featureDim;j++)
                            *buf++ = sparseVlat[i+j*featureDim];
                    }
                }
                return (featureDim*(featureDim+1))/2;
            }
            else if (format == "L") {
                if (buf) {
                    for (size_t i=0;i<featureDim;i++) {
                        for (size_t j=i+1;j<featureDim;j++)
                            *buf++ = sparseVlat[i+j*featureDim];
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
    
    void SparseVlatCluster::setEigenDecomposition(const float* eigenVectors , const float* eigenValues, size_t vectorsDim, size_t vectorsSize){
        if (vectorsDim != vectorsSize)
                retinThrowException("Not square matrix");
        if (vectorsDim != getTensorDim(1))
                retinThrowException2("Invalid dim %d != %d", vectorsDim, getTensorDim(1));
        
        U.resize(vectorsDim*vectorsDim);
        L.resize(vectorsDim);
        memcpy(&U[0], eigenVectors, vectorsDim*vectorsDim * sizeof(float));
        memcpy(&L[0], eigenValues, vectorsDim * sizeof(float));
    }

    SparseVlatBuilder::SparseVlatBuilder(size_t order,size_t dictSize,size_t featureDim): VlatBuilder<SparseVlatCluster>(order,dictSize,featureDim){

    }

    SparseVlatBuilder::~SparseVlatBuilder(){

    }
    
    void SparseVlatBuilder::setSparsityOn(int o){
        for (size_t c=0;c<clusters.size();c++) {
            clusters[c].setSparsityOn(o);
        }
    }

    void SparseVlatBuilder::setSparsityStrategy(int s){
        for (size_t c=0;c<clusters.size();c++) {
            clusters[c].setSparsityStrategy(s);
        }
    }

    void SparseVlatBuilder::setEnergySparsity(float e){
        for (size_t c=0;c<clusters.size();c++) {
            clusters[c].setEnergySparsity(e);
        }
    }

    void SparseVlatBuilder::setDimSparsity(size_t d){
        for (size_t c=0;c<clusters.size();c++) {
            clusters[c].setDimSparsity(d);
        }
    }
    
    size_t SparseVlatBuilder::getVlatDim(const std::string& format) const{
        size_t dim = 0;
        for (size_t c=0;c<clusters.size();c++) {
            dim += clusters[c].getVlat(NULL,format);
        }
        return dim;
    }

    void SparseVlatBuilder::getVlat(float* buffer,const std::string& format) const{
        size_t dim = 0;
        for (size_t c=0;c<clusters.size();c++) {
            dim += clusters[c].getVlat(buffer+dim,format);
        }
    }
    
    void SparseVlatBuilder::setEigenDecomposition(size_t cluster, const float* eigenVectors , const float* eigenValues, size_t vectorsDim, size_t vectorsSize){
        clusters[cluster].setEigenDecomposition(eigenVectors, eigenValues, vectorsDim, vectorsSize);
    }

}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/FloatMatrix.h"
#include "retin/toolbox/document/Parameters.h"


#include "retin/toolbox/document/basic/NativePointer.h"
#include "retin/toolbox/nativegenerator/NativeDocumentGenerator.h"

using namespace retin;
using namespace boost;
using namespace vlat;

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_SparseVlatBuilder_run
  (JNIEnv * env, jobject thisObj, jobject inputObj, jint degre, jstring formatObj)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureListRef> tensors; je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");
    vector<FloatMatrixRef> Ls; je.getDocumentField<FloatMatrix> (Ls,thisObj,"Ls");
    vector<FloatMatrixRef> Ps; je.getDocumentField<FloatMatrix> (Ps,thisObj,"Ps");
    string format = je.createString(formatObj);

    SparseVlatBuilder vlat(degre,centers->size(),centers->dim());
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int d=0;d<degre;d++) {
        FeatureListRef tensor = tensors[d];
        vlat.setMeanTensors(d+1,tensor->data(),tensor->size(),tensor->dim());
    }
    for (int i=0;i<Ls.size();i++) {
        vlat.setEigenDecomposition(i, Ps[i]->data(), Ls[i]->data(), Ps[i]->vectorSize(), Ps[i]->vectorCount());
    }
    
    vlat.setSparsityOn(je.getField<jint>(thisObj,"sparsityOn"));
    vlat.setSparsityStrategy(je.getField<jint>(thisObj,"sparsityStrategy"));
    vlat.setDimSparsity(je.getField<jint>(thisObj,"sparsityDim"));
    vlat.setEnergySparsity(je.getField<jfloat>(thisObj,"sparsityEnergy"));

    FeatureListRef input = je.createDocument<FeatureList > (inputObj);
    vlat.process(input->data(),input->size(),input->dim());

    FeatureRef outputRef = boost::make_shared<Feature>(vlat.getVlatDim(format));
    vlat.getVlat(outputRef->data(),format);

    return je.createObject (outputRef);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_SparseVlatBuilder_runNativeGenerator
  (JNIEnv *env, jobject thisObject, jobject nativePointerObj, jstring keyObj, jint degre, jstring formatObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    string key = je.createString(keyObj);
    
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObject,"centers");

    vector<FeatureListRef> tensors; 
    je.getDocumentField<FeatureList> (tensors,thisObject,"tensors");
    
    vector<FloatMatrixRef> Ls; je.getDocumentField<FloatMatrix> (Ls,thisObject,"Ls");
    vector<FloatMatrixRef> Ps; je.getDocumentField<FloatMatrix> (Ps,thisObject,"Ps");
           
    string format = je.createString(formatObj);
    
    
    NativePointerRef instance = je.createDocument<NativePointer>(nativePointerObj);
    NativeDocumentGenerator *ngd = instance->getPointerAs<NativeDocumentGenerator>();
    
    FeatureListRef features = boost::static_pointer_cast<FeatureList>(ngd->computeDocument(key));
    
    SparseVlatBuilder vlat(degre,centers->size(),centers->dim());
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int d=0;d<degre;d++) {
        FeatureListRef tensor = tensors[d];
        vlat.setMeanTensors(d+1,tensor->data(),tensor->size(),tensor->dim());
    }
    for (int i=0;i<Ls.size();i++) {
        vlat.setEigenDecomposition(i, Ps[i]->data(), Ls[i]->data(), Ps[i]->vectorSize(), Ps[i]->vectorCount());
    }
    
    vlat.setSparsityOn(je.getField<jint>(thisObject,"sparsityOn"));
    vlat.setSparsityStrategy(je.getField<jint>(thisObject,"sparsityStrategy"));
    vlat.setDimSparsity(je.getField<jint>(thisObject,"sparsityDim"));
    vlat.setEnergySparsity(je.getField<jfloat>(thisObject,"sparsityEnergy"));

    vlat.process(features->data(),features->size(),features->dim());

    FeatureRef outputRef = boost::make_shared<Feature>(vlat.getVlatDim(format));
    vlat.getVlat(outputRef->data(),format);

    return je.createObject (outputRef);
    
    RETIN_JAVA_METHOD_END
    return NULL;
}

#endif