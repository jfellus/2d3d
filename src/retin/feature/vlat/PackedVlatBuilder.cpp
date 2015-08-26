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

#include "PackedVlatBuilder.h"

#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"
#include "retin/toolbox/algebra/matrix.h"

#include "retin/toolbox/core/arrays.h"

using namespace std;
using namespace retin;
using namespace algebra;

namespace vlat {

void    PackedVlatCluster::init() {
    pca.init(getTensorDim(1));
}

void    PackedVlatCluster::add(const float* feature,size_t dim)
{
    size_t meanDim = getTensorDim(1);
    if (dim != meanDim)
        retinThrowException2("Invalid feature dim %d != %d",dim,meanDim);
    
    vector<double> temp(meanDim);
    const float* mean = getMeanTensor(1);
    if (mean) {
        for (size_t i=0;i<dim;i++)
            temp[i] = feature[i] - mean[i];
    }
    else {
        for (size_t i=0;i<dim;i++)
            temp[i] = feature[i];
    }

    pca.add(&temp[0],dim);
}

PackedVlatBuilder::PackedVlatBuilder(size_t order,size_t dictSize,size_t featureDim)
             : VlatBuilder<PackedVlatCluster>(order,dictSize,featureDim)
{
    if (order != 2)
        retinThrowException1("Invalid order %d for PackedVlat",order);
}

PackedVlatBuilder::~PackedVlatBuilder()
{
}

PackedVlat*     PackedVlatBuilder::createPackedVlat(double mse)
{
    size_t featureDim = 0;

    // Calcule les valeurs propres
    for (size_t c=0;c<clusters.size();c++) {
        size_t dim = clusters[c].getTensorDim(1);
        if (featureDim == 0)
            featureDim = dim;
        else if (featureDim != dim)
            retinThrowException2("Different clusters dim %d != %d",dim,featureDim);
        PCA& pca = clusters[c].pca;
        pca.computeEigens();
    }
    
    // Range toutes les valeurs propres
    double norm2 = 0;
    vector< pair<size_t,float> > order;
    for (size_t c=0;c<clusters.size();c++) {
        const std::vector<PCA::Eigen>& eigens = clusters[c].pca.getEigens();            
        for (size_t e=0;e<eigens.size();e++) {
            double x = eigens[e].getValue();
            order.push_back(make_pair(e+c*featureDim,x*x));
            norm2 += x*x;
        }
    }
    vsort_inc(order);
    norm2 = sqrt(norm2);

    // Retire des vecteurs tant qu'on ne commet pas trop d'erreurs
    size_t featureCount = order.size();
    size_t bagCount = clusters.size();
    double error = 0;
    vector<bool> selected(featureDim*bagCount,true);
    for (size_t i=0;i<order.size();i++) {
        if (sqrt((error+order[i].second)) > mse*norm2)
            break;
        error += order[i].second;
        selected[order[i].first] = false;
        featureCount --;
    }

    // Forme le VLAT packé
    PackedVlat* pvlat = new PackedVlat(featureDim,featureCount,bagCount);
    size_t count = 0;
    for (size_t c=0;c<clusters.size();c++) {
        PCA& pca = clusters[c].pca;
        const std::vector<PCA::Eigen>& eigens = pca.getEigens();
        pvlat->setBagInitialSize(c,pca.getCount());
        size_t bagSize = 0;
        for  (size_t e=0;e<eigens.size();e++) {
            if (selected[e+c*featureDim]) {
                if (count >= featureCount)
                    retinThrowException2("Oo %d != %d",count,featureCount);
                float* data = pvlat->feature(count++);
                for (size_t i=0;i<featureDim;i++) {
                    data[i] = (float) (sqrt(eigens[e].getValue())*eigens[e].getVector()[i]);
                }
                bagSize ++;
            }
        }
        pvlat->setBagSize(c,bagSize);
    }

    //matrix_print (cout,pvlat->data(),featureDim,featureCount);

    return pvlat;
}

FastPackedVlat*     PackedVlatBuilder::createFastPackedVlat(double mse) {
    size_t featureDim = 0;

    // Calcule les valeurs propres
    for (size_t c=0;c<clusters.size();c++) {
        size_t dim = clusters[c].getTensorDim(1);
        if (featureDim == 0)
            featureDim = dim;
        else if (featureDim != dim)
            retinThrowException2("Different clusters dim %d != %d",dim,featureDim);
        PCA& pca = clusters[c].pca;
        pca.computeEigens();
    }

    // Range toutes les valeurs propres
    double norm2 = 0;
    vector< pair<size_t,float> > order;
    for (size_t c=0;c<clusters.size();c++) {
        const std::vector<PCA::Eigen>& eigens = clusters[c].pca.getEigens();
        for (size_t e=0;e<eigens.size();e++) {
            double x = eigens[e].getValue();
            order.push_back(make_pair(e+c*featureDim,x*x));
            norm2 += x*x;
        }
    }
    vsort_inc(order);
    norm2 = sqrt(norm2);

    // Retire des vecteurs tant qu'on ne commet pas trop d'erreurs
    size_t featureCount = order.size();
    size_t bagCount = clusters.size();
    double error = 0;
    vector<bool> selected(featureDim*bagCount,true);
    for (size_t i=0;i<order.size();i++) {
        if (sqrt((error+order[i].second)) > mse*norm2)
            break;
        error += order[i].second;
        selected[order[i].first] = false;
        featureCount --;
    }

    // Forme le VLAT packé
    FastPackedVlat* pvlat = new FastPackedVlat(featureDim,featureCount,bagCount);
    size_t count = 0;
    for (size_t c=0;c<clusters.size();c++) {
        PCA& pca = clusters[c].pca;
        const std::vector<PCA::Eigen>& eigens = pca.getEigens();
        pvlat->setBagInitialSize(c,pca.getCount());
        size_t bagSize = 0;
        for  (size_t e=0;e<eigens.size();e++) {
            if (selected[e+c*featureDim]) {
                if (count >= featureCount)
                    retinThrowException2("Oo %d != %d",count,featureCount);
                float* data = pvlat->feature(count++);
                for (size_t i=0;i<featureDim;i++) {
                    data[i] = (float) (sqrt(eigens[e].getValue())*eigens[e].getVector()[i]);
                }
                bagSize ++;
            }
        }
        pvlat->setBagSize(c,bagSize);
    }

    //matrix_print (cout,pvlat->data(),featureDim,featureCount);

    return pvlat;
}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

using namespace vlat;

#include "PackedVlatDocument.h"

#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_PackedVlatBuilder_run
  (JNIEnv * env, jobject thisObj, jobject inputObj, jint degre, jdouble mse)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef centers = je.getDocumentField<FeatureList> (thisObj,"centers");
    vector<FeatureListRef> tensors; je.getDocumentField<FeatureList> (tensors,thisObj,"tensors");

    PackedVlatBuilder builder(degre,centers->size(),centers->dim());
    builder.setCenters(centers->data(),centers->size(),centers->dim());
    for (int d=0;d<degre;d++) {
        FeatureListRef tensor = tensors[d];
        builder.setMeanTensors(d+1,tensor->data(),tensor->size(),tensor->dim());
    }

    FeatureListRef input = je.createDocument<FeatureList > (inputObj);
    builder.process(input->data(),input->size(),input->dim());

    auto_ptr<PackedVlat> pvlat (builder.createPackedVlat(mse));
    PackedVlatDocumentRef outputRef = boost::make_shared<PackedVlatDocument>(pvlat.get());
    return je.createObject (outputRef);

    RETIN_JAVA_METHOD_END
    return NULL;
}

#endif
