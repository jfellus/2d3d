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
#include "CompEig.h"



#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/FloatMatrix.h"
#include "retin/toolbox/document/basic/NativePointer.h"
#include "retin/toolbox/document/list/FeatureList.h"

#include "retin/toolbox/database/Table.h"
#include "retin/toolbox/database/JavaManager.h"

#include "retin/toolbox/algebra/matrix_float.h"

#include "retin/toolbox/core/work_set.h"
#include "retin/toolbox/core/work_runnable.h"

#include "retin/toolbox/nativegenerator/NativeDocumentGenerator.h"
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace retin;
using namespace boost;

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_CompEig_compEigMatrices
  (JNIEnv * env, jobject thisObject){
    JavaEnv je(env);
    
    jint dimFeat = je.getField<jint >(thisObject, "dimFeat");
    
    FloatMatrixRef cov = je.getDocumentField<FloatMatrix>(thisObject, "cov");
    FloatMatrixRef mean = je.getDocumentField<FloatMatrix>(thisObject, "mean");
    
    FloatMatrixRef eigVectors = boost::make_shared<FloatMatrix>(dimFeat,dimFeat);
    FloatMatrixRef eigValues =  boost::make_shared<FloatMatrix>(dimFeat,  1);
    
    memcpy(eigVectors->data(), cov->data(), dimFeat*dimFeat*sizeof(float));
    matrix_eigSym_float (eigVectors->data(), eigValues->data(), dimFeat);
    matrix_sortEig_float (eigVectors->data(), eigValues->data(), dimFeat);
    
    je.setDocumentField<FloatMatrix >(thisObject, "eigValues", eigValues);
    je.setDocumentField<FloatMatrix >(thisObject, "eigVectors", eigVectors);    
}

static boost::mutex the_mutex;

class extractCompCovMatrix : public work_runnable{
public:
    
    extractCompCovMatrix(){
        
    }
    
    FloatMatrixRef mean;
    FloatMatrixRef cov;
    size_t *counter;
    string docKey;
    
    jint maxFeatPerDoc;
    jfloat pcFeatPerDoc;
    
    NativeDocumentGenerator *ngd;
    
    void run(){
        cout << docKey << endl;
        
        FeatureListRef res = boost::static_pointer_cast<FeatureList>(ngd->computeDocument(docKey));
        res->subRandomFeatureList(pcFeatPerDoc, maxFeatPerDoc);
        FloatMatrixRef covTemp = make_shared<FloatMatrix > (res->dim(),res->dim());
        FloatMatrixRef meanTemp = make_shared<FloatMatrix > (res->dim(),1);
        memset(covTemp->data(), 0, sizeof(float)*res->dim()*res->dim());
        memset(meanTemp->data(), 0, sizeof(float)*res->dim());
        for(int i = 0 ; i < res->size(); i++){
            vector_add_float (meanTemp->data(), res->data(i), res->dim());
            matrix_Cpaat_float (covTemp->data(), res->data(i), res->dim());
        }
        boost::mutex::scoped_lock lock(the_mutex);
        *counter += res->size();
        vector_add_float (mean->data(), meanTemp->data(), res->dim());
        vector_add_float (cov->data(), covTemp->data(), res->dim()*res->dim());
    }
};

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_CompEig_compCovMatrix
  (JNIEnv * env, jobject thisObject, jobject natPtrObj, jobjectArray keysObj){
    JavaEnv je(env);
    
    jint dimFeat = je.getField<jint >(thisObject, "dimFeat");
    
    FloatMatrixRef mean = make_shared<FloatMatrix > (dimFeat, 1);
    memset(mean->data(), 0, sizeof(float)*dimFeat);
    FloatMatrixRef cov =  make_shared<FloatMatrix > (dimFeat, dimFeat);
    memset(cov->data(), 0, sizeof(float)*dimFeat*dimFeat);
    
    size_t counter = 0;
    
    NativePointerRef instance = je.createDocument<NativePointer>(natPtrObj);
    NativeDocumentGenerator *ngd = instance->getPointerAs<NativeDocumentGenerator>();
    
    jint maxFeatPerDoc = je.getField<jint >(thisObject, "maxFeatPerDoc");
    jfloat pcFeatPerDoc = je.getField<jfloat >(thisObject, "pcFeatPerDoc");
    
    vector<string> keys = je.createStringVector(keysObj);
    
    work_pool workpool(32);
    work_set workset(&workpool);
    
    vector<boost::shared_ptr<extractCompCovMatrix> > extractorsList;
    for(vector<string>::iterator it = keys.begin(); it!=keys.end(); ++it)
    {
        //cout << *it << endl;
        boost::shared_ptr<extractCompCovMatrix > t(new extractCompCovMatrix());
        t->ngd = ngd;
        t->mean = mean;
        t->cov = cov;
        t->counter = &counter;
        t->docKey = *it;
        t->maxFeatPerDoc = maxFeatPerDoc;
        t->pcFeatPerDoc = pcFeatPerDoc;
        
        workset.pushRunnable(*t);
        
        extractorsList.push_back(t);
    }
    
    workset.join();
    workset.workpool()->destroy();
    
    extractorsList.clear();
    
    
    FloatMatrixRef temp = make_shared<FloatMatrix > (dimFeat, dimFeat);
    memset(temp->data(), 0, sizeof(float)*dimFeat*dimFeat);
    matrix_Cpaat_float (temp->data(), mean->data(), dimFeat);
    
    vector_sdiv_float (temp->data(), counter, dimFeat * dimFeat);
    
    vector_sub_float (cov->data(), temp->data(), dimFeat * dimFeat);
    
    vector_sdiv_float (cov->data(), counter, dimFeat * dimFeat);
    vector_sdiv_float (mean->data(), counter, dimFeat);
    
    je.setDocumentField<FloatMatrix >(thisObject, "mean", mean);
    je.setDocumentField<FloatMatrix >(thisObject, "cov", cov);
}


#endif //RETIN_DISABLE_JAVA
