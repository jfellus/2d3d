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

#ifndef __vlat__StandardPcaVlatBuilder_h__
#define __vlat__StandardPcaVlatBuilder_h__

#include "retin/toolbox/core/core.h"
#include "retin/toolbox/document/basic/Feature.h"
#include "retin/toolbox/document/basic/Integer.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/Matrix.h"

namespace vlat {
    
class StandardPcaVlatCluster
{
protected:
    
    size_t dimPrim;
    size_t dimDual;
    
    retin::FeatureRef meanPrim;
    
    retin::MatrixRef featc;
    
    retin::MatrixRef proj;
    
    retin::MatrixRef var;
    retin::MatrixRef invsqrtvar;
    
    retin::MatrixRef featDual;
    
    retin::MatrixRef vlat;
    
    unsigned int counter;
    
    bool whitening;
    
    int degre;
    
public:
    StandardPcaVlatCluster(retin::FeatureRef meanPrim, retin::MatrixRef proj, 
            retin::MatrixRef var, size_t dimPrim, size_t dimDual, bool whitening, int degre = 2);

    retin::FeatureRef  getData(std::string & format);
    size_t             getDim(std::string & format);
    void        add(retin::FeatureRef feat);
    void        add(const float * f, size_t dim);
};

typedef boost::shared_ptr<StandardPcaVlatCluster> StandardPcaVlatClusterRef;

class StandardPcaVlatBuilder
{
protected:
    std::vector<StandardPcaVlatClusterRef> clusters;
    retin::FeatureListRef center;
    int degre;
public:
        StandardPcaVlatBuilder(retin::FeatureListRef center, 
                std::vector<retin::FeatureListRef> tensors, 
                std::vector<retin::MatrixRef> vectors, 
                std::vector<retin::MatrixRef> values,
                std::vector<retin::IntegerRef> dims, bool whitening, int nbCluster, int degre = 2):center(center),degre(degre){
                    retin::FeatureListRef meansPrim = tensors[0];
                    for(int i = 0 ; i < nbCluster; i++){
                        retin::MatrixRef proj = vectors[i];
                        retin::MatrixRef var = values[i];
                        StandardPcaVlatClusterRef t = 
                         boost::make_shared<StandardPcaVlatCluster>(
                                meansPrim->getFeature(i), proj, var, 
                                proj->vectorSize(), dims[i]->get(), whitening, degre);
                        clusters.push_back(t);
                    }
                }
        
        void process(retin::FeatureListRef features);
        size_t getDim(std::string & format);
        retin::FeatureRef getVlat(std::string & format);

};


}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_StandardPcaVlatBuilder_run
  (JNIEnv *, jobject, jobject, jint, jstring);
JNIEXPORT jobject JNICALL Java_retin_feature_vlat_StandardPcaVlatBuilder_runNativeGenerator
  (JNIEnv *, jobject, jobject, jstring, jint, jstring);

}

#endif // RETIN_DISABLE_JAVA

#endif //__vlat__StandardPcaVlatBuilder_h__
