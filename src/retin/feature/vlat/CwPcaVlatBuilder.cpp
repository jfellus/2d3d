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

#include "CwPcaVlatBuilder.h"

#include "retin/toolbox/algebra/matrix.h"

using namespace std;

namespace vlat {

PcaProjector::PcaProjector() : featureDim(0),dirCount(0),meanTensor2(NULL) {

}

PcaProjector::~PcaProjector() {
    if (meanTensor2)
        delete[] meanTensor2;
}

void PcaProjector::setParams(
    const float* dir_,size_t dirCount_,size_t dirDim,
    const float* mean_,size_t meanDim,const float* var_,size_t varDim)
{
     if (dirCount == 0)
         dirCount = dirCount_;
     else if (dirCount != dirCount_)
        retinThrowException2("Invalid dir count %d != %d",dirCount,dirCount_);
     if (featureDim == 0)
         featureDim = dirDim;
     else if (dirDim != featureDim)
        retinThrowException2("Invalid dir dim %d != %d",dirDim,featureDim);
     dir = dir_;

     if (meanDim != featureDim)
        retinThrowException2("Invalid mean dim %d != %d",meanDim,featureDim);
     mean = mean_;

     if (varDim != dirCount)
        retinThrowException2("Invalid var dim %d != %d",varDim,featureDim);
     var = var_;

     meanTensor2 = new float[dirCount*dirCount];
     memset(meanTensor2,0,dirCount*dirCount*sizeof(float));
     for (size_t i=0;i<dirCount;i++)
         meanTensor2[i+i*dirCount] = 1;
}


void PcaProjector::project(float* output,const float* input, size_t dim) const
{
    if (dim != featureDim)
        retinThrowException2("Invalid feature dim %d != %d",dim,featureDim);
    vector<float> temp(featureDim);
    vector_linear_float(&temp[0],input,-1,mean,featureDim);
    for (size_t k = 0; k < dirCount; k++) {
        float w = 1 / sqrt(var[k]);
        output[k] = w * vector_ps_float(&temp[0], dir+k*featureDim, featureDim);
    }
}

CwPcaVlatBuilder::CwPcaVlatBuilder(size_t order,size_t dictSize,size_t featureDim)
    : StandardVlatBuilder(order,dictSize,featureDim),
        projectors(dictSize)
{

}

void CwPcaVlatBuilder::setPcaParams(size_t cluster,
    const float* dir,size_t dirCount,size_t dirDim,
    const float* mean,size_t meanDim,const float* var,size_t varDim)
{
    projectors[cluster].setParams(dir,dirCount,dirDim,mean,meanDim,var,varDim);
    clusters[cluster].setTensorDims(dirCount);
    clusters[cluster].setMeanTensor(2,projectors[cluster].getMeanTensor2(),dirCount*dirCount);
}


void CwPcaVlatBuilder::add (size_t c,const float* data,size_t dim) {
    PcaProjector& projector = projectors[c];
    vector<float> temp(projector.getProjectSize());
    projector.project(&temp[0],data,dim);
    StandardVlatBuilder::add(c,&temp[0],temp.size());
}

}


////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/algebra/algebra.h"
#include "retin/toolbox/algebra/pca.h"

#include "retin/toolbox/core/system.h"

#include "retin/toolbox/database/Table.h"
#include "retin/toolbox/database/JavaManager.h"

#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/Matrix.h"

#include <boost/thread.hpp>
#include <boost/format.hpp>

using namespace retin;
using namespace boost;
using namespace vlat;


std::string  time_format(unsigned long t) {
    std::string s;
    int sec = t % 60;
    t /= 60;
    int min = t % 60;
    t /= 60;
    int hour = t % 24;
    t /= 24;
    int day = t;
    if (day > 0)
        s += (boost::format("%d day(s) ")%day).str();
    if (hour > 0)
        s += (boost::format("%02d:%02d:%02d")%hour%min%sec).str();
    else if (min > 0)
        s += (boost::format("%02d:%02d")%min%sec).str();
    else
        s += (boost::format("%02d")%sec).str();
    return s;
}

class CwPcaDictBuilder : public MatrixTableParser
{
    Matrix dict;
    algebra::SynchronizedPCA* pcas;
    double time0,time1;
public:
    CwPcaDictBuilder(const FeatureList& centers) :
        dict(centers.dim(),centers.size())
    {
        for (int j=0;j<centers.size();j++)
        for (int i=0;i<centers.dim();i++) {
            dict(i,j) = centers(i,j);
        }
        pcas = new algebra::SynchronizedPCA[dict.vectorCount()];
        time0 = retin::getdaytime();
        time1 = time0;
    }
    ~CwPcaDictBuilder() {
        delete[] pcas;
    }

    size_t  dictSize() const { return dict.vectorCount(); }

    virtual void    process (MatrixRef& features) {
        double time = retin::getdaytime();
        if ( (time-time1) > 1) {
            cout << "CwPca: " << getDocCount() << " documents; " << time_format(time-time0) << endl;
            time1 = time;
        }
        for (size_t i=0;i<(size_t)features->vectorCount();i++)
        {
            if (features->vectorSize() != dict.vectorSize())
                retinThrowException2("Bad dim %d != %d",features->vectorSize(),dict.vectorSize());
            const double* data = features->data(i);
            size_t c = matrix_argmin_l2_double(data,dict.data(),dict.vectorSize(),dict.vectorCount());
            pcas[c].add(data,features->vectorSize());
        }
    }

    void    center() {
        for (int c=0;c<dict.vectorCount();c++)
            pcas[c].center();
    }
    void    run(int maxDim,double maxEnergy) {
        for (int c=0;c<dict.vectorCount();c++)
            pcas[c].run(maxDim,maxEnergy);
    }

    FeatureRef  getMean(size_t c) const {
        const double* mean = pcas[c].getMean();
        FeatureRef feature = make_shared<Feature>(pcas[c].getDim());
        for (size_t i=0;i<(size_t)feature->dim();i++)
            feature->setValue(i,mean[i]);
        return feature;
    }
    FeatureRef  getVar(size_t c) const {
        const std::vector<algebra::PCA::Eigen>& eigens = pcas[c].getEigens();
        FeatureRef feature = make_shared<Feature>(eigens.size());
        for (size_t i=0;i<(size_t)feature->dim();i++)
            feature->setValue(i,eigens[i].getValue());
        return feature;
    }
    FeatureListRef  getDir(size_t c) const {
        const std::vector<algebra::PCA::Eigen>& eigens = pcas[c].getEigens();
        FeatureListRef feature = make_shared<FeatureList>(pcas[c].getDim(),eigens.size());
        for (size_t j=0;j<(size_t)feature->size();j++)
            for (size_t i=0;i<(size_t)feature->dim();i++)
                (*feature)(i,j) = eigens[j].getVector()[i];
        return feature;
    }
};

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPcaVlatBuilder_compDict
  (JNIEnv * env, jobject thisObj, jobject tableObj, jobjectArray keysObj, jint maxDim, jdouble maxEnergy) {

    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList>(thisObj,"centers");
    TableRef table = JavaManager::GetTable( je,tableObj );
    vector<string> keys = je.createStringVector(keysObj);

    work_pool pool;
    work_set set(&pool);
    pool.run();

    CwPcaDictBuilder parser(*centers);
    table->parseDocuments( keys,&parser,set );
    parser.center();
    parser.run(maxDim,maxEnergy);

    vector<FeatureListRef> dirs;
    vector<FeatureRef> means,vars;
    for (size_t c=0;c<parser.dictSize();c++) {
        dirs.push_back(parser.getDir(c));
        means.push_back(parser.getMean(c));
        vars.push_back(parser.getVar(c));
    }

    je.setDocumentField(thisObj,"dirs",dirs);
    je.setDocumentField(thisObj,"means",means);
    je.setDocumentField(thisObj,"vars",vars);

    RETIN_JAVA_METHOD_END
    return NULL;

}

class MeanVlatTableParser : public FeatureListTableParser
{
    CwPcaVlatBuilder& vlat;
    string          format;
    size_t          dim;
    vector<double>  meanVlat;
    double          time0,time1;
public:
    MeanVlatTableParser(CwPcaVlatBuilder& vlat,const string& format) : vlat(vlat),format(format) {
        dim = vlat.getVlatDim(format);
        meanVlat.resize(dim);
        time0 = retin::getdaytime();
        time1 = time0;
    }
    virtual void    process (FeatureListRef& features) {
        double time = retin::getdaytime();
        if ( (time-time1) > 1) {
            cout << "MeanVlat: " << getDocCount() << " documents; " << time_format(time-time0) << endl;
            time1 = time;
        }

        vlat.process(features->data(),features->size(),features->dim());
        vector<float> temp(dim);
        vlat.getVlat(&temp[0],format);
        for (size_t i=0;i<dim;i++)
            meanVlat[i] += temp[i];
    }
    FeatureRef      getMeanVlat() {
        double fMin = 1E99, fMax = -1E99;
        FeatureRef output = boost::make_shared<Feature>(dim);
        for (size_t i=0;i<dim;i++) {
            double x = meanVlat[i];
            if (x < fMin) fMin = x;
            if (x > fMax) fMax = x;
            (*output)[i] = (float)x;
        }
        cout << "min:" << fMin << " max:" << fMax << endl;
        return output;
    }
};


JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPcaVlatBuilder_compMeanVlat
  (JNIEnv * env, jobject thisObj, jobject tableObj, jobjectArray keysObj, jint order, jstring formatObj) {

    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList>(thisObj,"centers");
    vector<FeatureRef> means; je.getDocumentField<Feature> (means,thisObj,"means");
    vector<FeatureRef> vars; je.getDocumentField<Feature> (vars,thisObj,"vars");
    vector<FeatureListRef> dirs; je.getDocumentField<FeatureList> (dirs,thisObj,"dirs");

    CwPcaVlatBuilder vlat(order,centers->size(),centers->dim());
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int c=0;c<centers->size();c++) {
        FeatureListRef dir = dirs[c];
        FeatureRef mean = means[c];
        FeatureRef var = vars[c];
        vlat.setPcaParams(c,dir->data(),dir->size(),dir->dim(),mean->data(),mean->dim(),var->data(),var->dim());
    }

    TableRef table = JavaManager::GetTable( je,tableObj );
    vector<string> keys = je.createStringVector(keysObj);
    string format = je.createString(formatObj);
    format = "full";
    MeanVlatTableParser parser(vlat,format);
    table->parseDocuments(keys,&parser);

    return je.createObject ( parser.getMeanVlat() );

    RETIN_JAVA_METHOD_END
    return NULL;

}

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_CwPcaVlatBuilder_compVlat
  (JNIEnv * env, jobject thisObj, jobject inputObj, jint order, jstring formatObj)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureRef> means; je.getDocumentField<Feature> (means,thisObj,"means");
    vector<FeatureRef> vars; je.getDocumentField<Feature> (vars,thisObj,"vars");
    vector<FeatureListRef> dirs; je.getDocumentField<FeatureList> (dirs,thisObj,"dirs");
    string format = je.createString(formatObj);

    CwPcaVlatBuilder vlat(order,centers->size(),centers->dim());
    vlat.setCenters(centers->data(),centers->size(),centers->dim());
    for (int c=0;c<centers->size();c++) {
        FeatureListRef dir = dirs[c];
        FeatureRef mean = means[c];
        FeatureRef var = vars[c];
        vlat.setPcaParams(c,dir->data(),dir->size(),dir->dim(),mean->data(),mean->dim(),var->data(),var->dim());
    }

    FeatureListRef input = je.createDocument<FeatureList > (inputObj);
    vlat.process(input->data(),input->size(),input->dim());

    FeatureRef outputRef = boost::make_shared<Feature>(vlat.getVlatDim(format));
    vlat.getVlat(outputRef->data(),format);

    return je.createObject (outputRef);

    RETIN_JAVA_METHOD_END
    return NULL;

}


#endif
