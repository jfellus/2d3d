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
#include "ElbgQuantizer.h"

#include "retin/toolbox/core/string_retin.h"

#include <vector>
#include <memory>

using namespace std;
using namespace retin;

namespace quantization {

void    ElbgQuantizer::run(const float* data,size_t dataDim,size_t dataCount)
{
    NaiveSplittingQuantizer::run(data,dataDim,dataCount);

    size_t ite = 0;
    double D1 = D;
    while (true) {
        // Nouveaux clusters
        computeClusters();

        // Distortion globale
        size_t cp = getNonEmptyClusterCount();
        // Test d'arrêt
        if (verboseLevel >= 1)
            std::cout << "Distortion = " << (D/sampleCount)
                      << " Diff = " << (fabs(D1 - D) / D)
                      << " Iteration " << ite
                      << " #Clusters = " << cp << "/" << clusters.size()
                      << std::endl;
        ite++;
        if (ite >= maxIterations
        || (D >= minDistortion && (D <= maxDistortion || fabs(D1 - D) / D < maxDistortionDiff)))
            break;
        D1 = D;

        // Optimisation ELBG
        for (size_t c = 0; c < clusters.size(); c++) {
            clusters[c]->setSplit(false);
            clusters[c]->setFusion(false);
        }
        fillEmptyClusters();
        if (ite < 3)
            optimize();
        //dictionnaryReduction();

        // Nouveaux centres
        computeCenters();
    }

    while (true) {
        int c;
        for (c = clusters.size() - 1; c >= 0; c--) {
            if (clusters[c]->isEmpty()) {
                removeEmptyCluster(c);
                break;
            }
        }
        if (c < 0)
            break;
    }
}

class   ClusterConqueror : public thread_pool::conqueror {
    const float* A;
    AbstractQuantizer* qt;
public:
    ClusterConqueror(AbstractQuantizer* qt,const float* A) : A(A),qt(qt) {
    }
    virtual void    conquer (size_t i0,size_t n)
    {
        size_t nc = qt->getClusterCount();
        vector<AbstractCluster*>  clusters = qt->getAbstractClusters();

        for (size_t j=0;j<n;j++)
        {
            size_t i = i0+j;
            const float* data = qt->getSample(i);
            size_t  cMin = qt->getLabel(i);
            float dMin = clusters[cMin]->distTo(data);
            const float* a = A+cMin*nc;
            for (size_t c=0;c<nc;c++)
            {
                if (a[c] < 4*dMin)
                {
                    float d = clusters[c]->distTo(data);
                    if (d < dMin)
                    {
                            dMin = d;
                            cMin = c;
                            a = A+cMin*nc;
                    }
                }
            }
            clusters[cMin]->addSample(i,dMin);
            qt->setLabel(i,cMin);
        }

    }
};

void	ElbgQuantizer::computeClusters ()
{
    size_t nc = getClusterCount();
    std::vector<AbstractCluster*> clusters = getAbstractClusters();
    std::vector<float> A(nc*nc);
    for (size_t c1=0;c1<nc;c1++)
    for (size_t c2=0;c2<c1;c2++)
    {
        A[c1+c2*nc] = A[c2+c1*nc] = clusters[c1]->distTo(clusters[c2]->getCenter());
    }
    for (size_t c=0;c<nc;c++)
    {
        clusters[c]->clear();
        A[c+c*nc] = 1E99;
    }

    ClusterConqueror conqueror(this,&A[0]);
    thread_pool_set workset(workpool);
    workset.push_conquerors(conqueror,0,sampleCount);
    workset.join();
 

    D = computeDistortion();
}

void	ElbgQuantizer::computeCenters ()
{
    size_t nc = getClusterCount();
    vector<size_t> weights(nc);
    vector<double> centers(nc*featureDim);
    for (size_t i=0;i<sampleCount;i++) {
        size_t c = labels[i];
        const float* data = getSample(i);
        for (size_t k=0;k<featureDim;k++) {
            centers[k+c*featureDim] += data[k];
        }
        weights[c] ++;
    }
    for (size_t c=0;c<nc;c++) {
        double w = weights[c];
        if (w > 0) {
            w = 1 / w;
        }
        float* center = getAbstractCluster(c)->getCenter();
        for (size_t k=0;k<featureDim;k++) {
            center[k] = w*centers[k+c*featureDim];
        }
    }
}

void	ElbgQuantizer::removeCluster (size_t i,size_t l)
{
    if (i == l)
        return;
    if (i < l)
        std::swap(i, l);

    D -= clusters[l]->getDistortion() + clusters[i]->getDistortion();
    auto_ptr<NaiveSplittingCluster> clusterF(new NaiveSplittingCluster(mainData,featureDim));
    clusterF->mergeClusters(*clusters[i], *clusters[l]);
    setCluster(l,clusterF.release());
    D += clusters[l]->getDistortion();

    delete clusters[i];
    clusters.erase(clusters.begin() + i);

    for (size_t j = 0; j < labels.size(); j++) {
        size_t label = labels[j];
        if (label > i) labels[j]--;
        else if (label == i) labels[j] = l;
    }
}


void	ElbgQuantizer::removeEmptyCluster (size_t i)
{
    if (clusters[i]->getSize() != 0)
        return;

    delete clusters[i];
    clusters.erase(clusters.begin()+i);

    for (size_t j=0;j<labels.size();j++) {
        if (labels[j] > (int) i) labels[j] --;
    }
}

bool ElbgQuantizer::swap(size_t l, size_t i, size_t p)
{
    if (verboseLevel >= 3)
        std::cout << string_format("Try (l,i,p) %d %d %d (dist=%f %f %f)",l,i,p,
           clusters[i]->getDistortion(),clusters[l]->getDistortion(),clusters[p]->getDistortion())
            << std::endl;

    // Distortion sans échange.
    float rDistortionSans;
    if (i != l)
        rDistortionSans = clusters[i]->getDistortion() + clusters[l]->getDistortion() + clusters[p]->getDistortion();
    else
        rDistortionSans = clusters[p]->getDistortion() + clusters[l]->getDistortion();
    // Si la classe i n'est pas vide, on l'unie à la classe l.
    auto_ptr<NaiveSplittingCluster> clusterF(new NaiveSplittingCluster(mainData,featureDim));
    if (i != l) {
        clusterF->mergeClusters(*clusters[i], *clusters[l]);
        if (clusterF->getDistortion() > rDistortionSans) {
            if (verboseLevel >= 3)
                std::cout << " -- Echec fusion" << std::endl;
            return false;
        }
    }

    // On découpe classe p en 2.
    auto_ptr<NaiveSplittingCluster> cluster1 (new NaiveSplittingCluster(mainData,featureDim));
    auto_ptr<NaiveSplittingCluster> cluster2 (new NaiveSplittingCluster(mainData,featureDim));
    clusters[p]->splitCluster(*cluster1, *cluster2);

    if (verboseLevel >= 3) {
        std::cout << string_format(" -- Distortion découpage (avant/après): %f -> %f",
                clusters[p]->getDistortion(),(cluster1->getDistortion() + cluster2->getDistortion())) << std::endl;
        std::cout << string_format(" -- Distortion totale: %f -> %f = %f + %f + %f",
                rDistortionSans,(cluster1->getDistortion() + cluster2->getDistortion() + clusterF->getDistortion()),
                cluster1->getDistortion(),cluster2->getDistortion(),clusterF->getDistortion()) << std::endl;
    }

    // On peut calculer la différence des distortions.
    if ((cluster1->getDistortion() + cluster2->getDistortion() + clusterF->getDistortion()) > rDistortionSans) {
        if (verboseLevel >= 3)
            std::cout << " -- Echec" << std::endl;
        return false;
    }
    // Ok, il est intéressant d'effectuer l'opération.

    if (verboseLevel >= 2)
        std::cout << " -- Swap gain: " 
        << (-(cluster1->getDistortion() + cluster2->getDistortion() + clusterF->getDistortion()) + rDistortionSans)
        << std::endl;

    // on met à jour .
    D += (cluster1->getDistortion() + cluster2->getDistortion() + clusterF->getDistortion()) - rDistortionSans;

    if (i != l) {
        clusterF->updateLabels(labels,l);
        setCluster(l,clusterF.release());
    }

    cluster1->updateLabels(labels,p);
    setCluster(p,cluster1.release());

    cluster2->updateLabels(labels,i);
    setCluster(i,cluster2.release());

    return true;
}

int ElbgQuantizer::findCenterL(size_t i) {
    int l = -1;
    if (clusters[i]->isEmpty() == 0)
        return i;
    float rMin(1E99);
    for (size_t k = 0; k < clusters.size(); k++)
        if (k != i) {
            float r = clusters[i]->distTo(clusters[k]->getCenter());
            if (r < rMin) {
                rMin = r;
                l = k;
            }
        }
    if (clusters[l]->isSplit() || clusters[l]->isFusion())
        return -1;
    return l;
}

int ElbgQuantizer::findCenterP(size_t l) {
    size_t p;
    float s, rDistortionMoy(D / clusters.size());
    for (p = 0, s = 0; p < clusters.size(); p++)
        if (clusters[p]->getDistortion() > rDistortionMoy && !clusters[p]->isFusion() && p != l)
            s += clusters[p]->getDistortion();
    if (s == 0)
        return -1;
    float r = s * float(rand() % 1000000) / 1000000;
    for (p = 0, s = 0; p < clusters.size(); p++)
        if (clusters[p]->getDistortion() > rDistortionMoy && !clusters[p]->isFusion() && p != l) {
            s += clusters[p]->getDistortion();
            if (s >= r)
                break;
        }
    return p;
}

void ElbgQuantizer::fillEmptyClusters() {
    for (size_t c = 0; c < clusters.size(); c++) {
        if (clusters[c]->isEmpty()) {
            int p = findCenterP(c);
            if (p >= 0)
                swap(c, c, p);
        }
    }
}

void ElbgQuantizer::optimize() {    
    for (size_t c = 0; c < clusters.size(); c++) {
        float dist = D / clusters.size();
        if (clusters[c]->getDistortion() < dist && !clusters[c]->isSplit() && !clusters[c]->isFusion()) {
            int l = findCenterL(c);
            if (l >= 0) {
                int p = findCenterP(l);
                if (p >= 0)
                    swap(l, c, p);
            }
        }
    }

}

}

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/Parameters.h"
#include "retin/toolbox/document/basic/Feature.h"
#include "retin/toolbox/document/basic/NativePointer.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/IndexMatrix.h"
#include "retin/toolbox/document/ImageFile.h"
#include "retin/toolbox/document/FeatureMatrix.h"
#include "retin/toolbox/document/FeatureMatrix.h"

#include "retin/toolbox/database/Table.h"
#include "retin/toolbox/database/JavaManager.h"

#include "retin/toolbox/core/work_set.h"
#include "retin/toolbox/core/work_runnable.h"

#include "retin/toolbox/nativegenerator/NativeDocumentGenerator.h"

#include "DiagGmmQuantizer.h"

#include <boost/bind.hpp>


using namespace quantization;

class SpaceType {
public:
    jfloat* data;
    SpaceType(float* data) : data(data) { }
    ~SpaceType() { delete[] data; }
};

void lbg_run(JavaEnv& je, jobject thisObject, jint dim, jfloat* data, jint size,work_pool* workpool=NULL) {

    SpaceType* S = new SpaceType(data);
    je.setPtrField<SpaceType> (thisObject, "spacePtr", S);

    ElbgQuantizer* lbg = new ElbgQuantizer(dim);
    je.setPtrField<ElbgQuantizer> (thisObject, "lbgPtr", lbg);

    ParametersRef params = je.getDocumentField<Parameters > (thisObject, "params");
    size_t dictSize = params->getAs<int>("maxWords", 256);
    lbg->setMaxWords(dictSize);
    lbg->setVerboseLevel(params->getAs<int>("verboseLevel", 0));
    lbg->setMaxIterations(params->getAs<int>("maxIterations", 10));
    lbg->setMinDistortion(params->getAs<double>("minDistortion", 0));
    lbg->setMaxDistortion(params->getAs<double>("maxDistortion", 1E-6));
    lbg->setMaxDistortionDiff(params->getAs<double>("maxDistortionDiff", 1E-4));

    lbg->setWorkPool(workpool);
    lbg->run(data, dim, size);

    if (!params->has("computeGMM"))
        return;

    vector<float> centers ( dim*dictSize );
    lbg->getCenters( &centers[0], dim );
    vector<float> mean2 ( dim*dim*dictSize );
    lbg->computeMeanTensors( &mean2[0], dim*dim, 2);

    DiagGmmQuantizer* em = new DiagGmmQuantizer(dim,dictSize);
    je.setPtrField<DiagGmmQuantizer> (thisObject, "gmmPtr", em);

    em->setVerboseLevel (params->getAs<int>("verboseLevel", 0));
    //em->setMaxIterations (params->getAs<int>("maxIterations", 10));
    em->setMaxIterations (10);
    em->setWorkPool (workpool);

    em->init(&centers[0],&mean2[0],dim,dictSize);
    em->run(data,dim,size);
}

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_release
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    je.setPtrField<ElbgQuantizer> (thisObject, "lbgPtr", (ElbgQuantizer*) NULL);
    je.setPtrField<DiagGmmQuantizer> (thisObject, "gmmPtr", (DiagGmmQuantizer*) NULL);
    je.setPtrField<SpaceType > (thisObject, "spacePtr", (SpaceType*) NULL);

    RETIN_JAVA_METHOD_END
}

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runFeatureList
(JNIEnv * env, jobject thisObject, jobject inputObj) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureListRef input = je.createDocument<FeatureList> (inputObj);
    lbg_run(je, thisObject, input->dim(), input->releaseData(), input->size());

    RETIN_JAVA_METHOD_END
}


JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runFeatureMatrix
(JNIEnv * env, jobject thisObject, jobject inputObj) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    FeatureMatrixRef input = je.createDocument<FeatureMatrix> (inputObj);
    lbg_run(je, thisObject, input->dim(), input->releaseData(), input->width() * input->height());

    RETIN_JAVA_METHOD_END
}

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runTable
(JNIEnv * env, jobject thisObject, jobject tableObj, jobjectArray keysObj) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    vector<string> keys = je.createStringVector(keysObj);
    TableRef table = JavaManager::GetTable(je, tableObj);
    ParametersRef params = je.getDocumentField<Parameters > (thisObject, "params");
    FeatureListRef input = table->getFeatureList(keys,params->getAs<float>("perFeaturesPerDoc",1.0f),params->getAs<int>("maxFeaturesPerDoc",0),params->getAs<int>("verboseLevel",0));
    lbg_run(je, thisObject, input->dim(), input->releaseData(), input->size());

    RETIN_JAVA_METHOD_END
}

class extractor : public work_runnable{
protected:
    unsigned int id;
    int verboseLevel;
    int maxFeaturesPerDoc;
    float perFeaturesPerDoc;
    string docKey;
    
    static unsigned int count;
    
    NativeDocumentGenerator &instance;
    
    FeatureListRef res;
public:
    extractor(NativeDocumentGenerator& instance, const string& docKey) : 
        verboseLevel(0), maxFeaturesPerDoc(0), 
        perFeaturesPerDoc(1.0f), docKey(docKey), instance(instance){id = count++;}
    
    void setVerboseLevel(int verboseLevel){this->verboseLevel = verboseLevel;}
    void setMaxFeaturesPerDoc(int maxFeaturesPerDoc){this->maxFeaturesPerDoc = maxFeaturesPerDoc;}
    void setPerFeaturesPerDoc(float perFeaturesPerDoc){this->perFeaturesPerDoc = perFeaturesPerDoc;}
    
    FeatureListRef getFeatureList(){return res;}
    
    static void resetCount(){ count = 0; }
    
    
public :
    void run(){        
        res = boost::static_pointer_cast<FeatureList>(instance.computeDocument(docKey));
        res->subRandomFeatureList(perFeaturesPerDoc, maxFeaturesPerDoc);
        if(verboseLevel > 0)
            cout << "...fin " << id << "..." << endl;
    }
};

unsigned int extractor::count=0;

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runNativeGeneratorTable
  (JNIEnv *env, jobject thisObject, jobject nativePointerObj, jobject tableObj, jobjectArray keysObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    vector<string> keys = je.createStringVector(keysObj);
    std::vector<DocumentRef> docs;
    //TableRef table = JavaManager::GetTable(je, tableObj);
    ParametersRef params = je.getDocumentField<Parameters> (thisObject, "params");
    
    NativePointerRef instance = je.createDocument<NativePointer>(nativePointerObj);
    NativeDocumentGenerator *ngd = instance->getPointerAs<NativeDocumentGenerator>();
    
    float perFeaturesPerDoc = params->getAs<float>("perFeaturesPerDoc",1.0f);
    int maxFeaturesPerDoc = params->getAs<int>("maxFeaturesPerDoc",0);
    int verboseLevel = params->getAs<int>("verboseLevel",0);
    
    cout <<maxFeaturesPerDoc << endl;

    work_pool workpool(32);
    //work_pool workpool(1);
    work_set workset(&workpool);
    
    // pour l'affichage
    extractor::resetCount();
    
    vector<boost::shared_ptr<extractor> > extractorsList;
    for(vector<string>::iterator it = keys.begin(); it!=keys.end(); ++it)
    {
        //cout << *it << endl;
        boost::shared_ptr<extractor> t(new extractor(*ngd, *it));
        t->setVerboseLevel(verboseLevel);
        t->setMaxFeaturesPerDoc(maxFeaturesPerDoc);
        t->setPerFeaturesPerDoc(perFeaturesPerDoc);
        
        workset.pushRunnable(*t);
        
        extractorsList.push_back(t);
    }
    
    if(verboseLevel>0)
                cout << "Compute SIFTs ..." << endl;
    
    workset.join();
    workset.workpool()->destroy();

    if(verboseLevel>0)
            cout << "Compute FeatureList ..." << endl;

    size_t sizeFeatureList = 0;
    for(vector<boost::shared_ptr<extractor> >::iterator it = extractorsList.begin(); it!=extractorsList.end(); ++it)
    {
        sizeFeatureList += (*it)->getFeatureList()->size();
    }
    
    //feature dimension depending on generator
    size_t dim = ngd->getDim();

    if(verboseLevel>0)
            cout << "number of features in the sample " << sizeFeatureList << " (" << sizeFeatureList*sizeof(jfloat)*dim/(1024*1024) << " Mo)" << endl;

    FeatureListRef featuresList;
    featuresList.reset(new FeatureList(dim, sizeFeatureList));

    size_t i=0;

    while (!extractorsList.empty())
    {
        boost::shared_ptr<extractor> t = extractorsList.back();
        
        FeatureListRef temp = t->getFeatureList();

        memcpy((*featuresList)[i], temp->data(), temp->size()*dim*sizeof(jfloat));
        i+=temp->size();

        extractorsList.pop_back();
    }
    
    lbg_run(je, thisObject, featuresList->dim(), featuresList->releaseData(), featuresList->size());

    RETIN_JAVA_METHOD_END
}

JNIEXPORT void JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_runSampledTable
(JNIEnv * env, jobject thisObject, jobject tableObj, jobjectArray keysObj, jint percent) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    vector<string> keys = je.createStringVector(keysObj);
    TableRef table = JavaManager::GetTable(je, tableObj);
    std::cout << "fetching sampled featureList (" << percent << "%)" << std::endl;
    FeatureListRef input = table->getFeatureList(keys, percent);
    std::cout << "LBG got featureListRef" << std::endl;

    lbg_run(je, thisObject, input->dim(), input->releaseData(), input->size());

    RETIN_JAVA_METHOD_END
}


JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterSizes
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer> (thisObject, "lbgPtr");
    vector<AbstractCluster*> clusters = qt->getAbstractClusters();
    FeatureRef sizes = boost::make_shared<Feature> (clusters.size());
    for (int i = 0; i < sizes->dim(); i++)
        (*sizes)(i) = clusters[i]->getSize();
    return je.createObject(sizes);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterDistortions
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer> (thisObject, "lbgPtr");
    vector<AbstractCluster*> clusters = qt->getAbstractClusters();
    FeatureRef dists = boost::make_shared<Feature> (clusters.size());
    for (int i = 0; i < dists->dim(); i++)
        (*dists)(i) = clusters[i]->getDistortion();
    return je.createObject(dists);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterVariances
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer > (thisObject, "lbgPtr");
    vector<AbstractCluster*> clusters = qt->getAbstractClusters();
    FeatureRef dists = boost::make_shared<Feature> (clusters.size());
    for (int i = 0; i < dists->dim(); i++)
        (*dists)(i) = clusters[i]->getDistortion() / clusters[i]->getSize();
    return je.createObject(dists);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterCenters
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer> (thisObject, "lbgPtr");
    vector<AbstractCluster*> clusters = qt->getAbstractClusters();
    FeatureListRef centers = boost::make_shared<FeatureList> (qt->getFeatureDim(), clusters.size());
    for (int c = 0; c < centers->size(); c++)
        for (int k = 0; k < centers->dim(); k++)
            (*centers)(k, c) = clusters[c]->getCenter()[k];
    return je.createObject(centers);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getClusterTensors
(JNIEnv * env, jobject thisObject, jint order) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer > (thisObject, "lbgPtr");
    vector<AbstractCluster*> clusters = qt->getAbstractClusters();
    size_t featureDim = qt->getFeatureDim();
    size_t tensorDim = 1;
    for (int i=0;i<order;i++)
        tensorDim *= featureDim;
    FeatureListRef tensors = boost::make_shared<FeatureList> (tensorDim, clusters.size());

    work_pool workpool;
    work_set workset(&workpool);
    for (size_t c=0;c<clusters.size();c++) {
        workset.push(boost::bind(&AbstractCluster::computeTensor,clusters[c],tensors->data(c),tensorDim,order));
    }
    workset.join();

    return je.createObject(tensors);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jdouble JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getDistortion
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer > (thisObject, "lbgPtr");
    return qt->getDistortion();

    RETIN_JAVA_METHOD_END
    return -1;
}


JNIEXPORT jintArray JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getLabels
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer > (thisObject, "lbgPtr");
    return je.createArray(qt->getLabels());

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getIndexMatrix
(JNIEnv * env, jobject thisObject, jint width, jint height) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer > (thisObject, "lbgPtr");
    vector<AbstractCluster*> clusters = qt->getAbstractClusters();
    const vector<int>& labels = qt->getLabels();
    IndexMatrixRef index = boost::make_shared<IndexMatrix > (width, height, clusters.size());
    jint* data = index->data();
    for (uint i = 0; i < labels.size(); i++)
        data[i] = labels[i];
    return je.createObject(index);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getFeatureMatrix
(JNIEnv * env, jobject thisObject, jint width, jint height) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    ElbgQuantizer* qt = je.getPtrField<ElbgQuantizer > (thisObject, "lbgPtr");
    vector<AbstractCluster*> clusters = qt->getAbstractClusters();
    const vector<int>& labels = qt->getLabels();
    uint p = qt->getFeatureDim();
    FeatureMatrixRef output = boost::make_shared<FeatureMatrix > (width, height, p);
    jfloat* data = output->data();
    for (uint i = 0; i < labels.size(); i++) {
        uint c = labels[i];
        for (uint k = 0; k < p; k++)
            data[k + i * p] = clusters[c]->getCenter()[k];
    }
    return je.createObject(output);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getGaussianWeights
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    DiagGmm& gmm = je.getPtrField<DiagGmmQuantizer>(thisObject, "gmmPtr")->getGmm();
    FeatureRef weights = boost::make_shared<Feature> (gmm.getGaussianCount());
    gmm.getWeights( weights->data(), gmm.getGaussianCount() );
    return je.createObject(weights);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getGaussianMeans
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    DiagGmm& gmm = je.getPtrField<DiagGmmQuantizer>(thisObject, "gmmPtr")->getGmm();
    FeatureListRef means = boost::make_shared<FeatureList> (gmm.getFeatureDim(),gmm.getGaussianCount());
    gmm.getMeans( means->data(), gmm.getFeatureDim(), gmm.getGaussianCount() );
    return je.createObject(means);

    RETIN_JAVA_METHOD_END
    return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_quantization_ElbgQuantizer_getGaussianVars
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);

    DiagGmm& gmm = je.getPtrField<DiagGmmQuantizer>(thisObject, "gmmPtr")->getGmm();
    FeatureListRef vars = boost::make_shared<FeatureList> (gmm.getFeatureDim(),gmm.getGaussianCount());
    gmm.getVars( vars->data(), gmm.getFeatureDim(), gmm.getGaussianCount() );
    return je.createObject(vars);

    RETIN_JAVA_METHOD_END
    return NULL;
}

#endif
