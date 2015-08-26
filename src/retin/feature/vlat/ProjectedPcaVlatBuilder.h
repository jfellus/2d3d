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

#ifndef __vlat__ProjectedPcaVlatBuilder_h__
#define __vlat__ProjectedPcaVlatBuilder_h__

#include "../../toolbox/core/core.h"
#include "../../toolbox/document/basic/Feature.h"
#include "../../toolbox/document/basic/Integer.h"
#include "../../toolbox/document/list/FeatureList.h"
#include "../../toolbox/document/Matrix.h"

namespace vlat {


    
class ProjectedPcaVlatCluster
{
protected:
    
/*    static int test;
    int id;*/
    
    size_t dimPrim;
    size_t dimDual;
    retin::FeatureRef meanPrim;
    
    retin::MatrixRef meanDual;
    
    retin::MatrixRef featc;
    
    retin::MatrixRef proj;
    
    retin::MatrixRef var;
    retin::MatrixRef invvar;
    
    retin::MatrixRef featDual;
    
    retin::MatrixRef tensor;
    
    unsigned int counter;
    
public:
    ProjectedPcaVlatCluster(retin::FeatureRef meanPrim, retin::MatrixRef proj, 
            retin::MatrixRef var, size_t dimPrim, size_t dimDual);

    retin::FeatureRef  getData(std::string centered);
    size_t             getDim();
    void        add(retin::FeatureRef feat);
    void        add(const float * f, size_t dim);
};

//int StandardPcaVlatCluster::test = 0;

typedef boost::shared_ptr<ProjectedPcaVlatCluster> ProjectedPcaVlatClusterRef;

class ProjectedPcaVlatBuilder
{
protected:
    std::vector<ProjectedPcaVlatClusterRef> clusters;
    retin::FeatureListRef center;
public:
        ProjectedPcaVlatBuilder(retin::FeatureListRef center, 
                std::vector<retin::FeatureListRef> tensors, 
                std::vector<retin::MatrixRef> vectors, 
                std::vector<retin::MatrixRef> values,
                std::vector<retin::IntegerRef> dims, int nbCluster):center(center){
                    retin::FeatureListRef meansPrim = tensors[0];
                    for(int i = 0 ; i < nbCluster; i++){
                        retin::MatrixRef proj = vectors[i];
                        retin::MatrixRef var = values[i];
                        ProjectedPcaVlatClusterRef t = 
                         boost::make_shared<ProjectedPcaVlatCluster>(
                                meansPrim->getFeature(i), proj, var, 
                                proj->vectorSize(), dims[i]->get());
                        clusters.push_back(t);
                    }
                }
        
        void process(retin::FeatureListRef features);
        size_t getDim();
        retin::FeatureRef getVlat(std::string centered);

};


}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_feature_vlat_ProjectedPcaVlatBuilder_run
  (JNIEnv *, jobject, jobject);
JNIEXPORT jobject JNICALL Java_retin_feature_vlat_ProjectedPcaVlatBuilder_runNativeGenerator
  (JNIEnv *, jobject, jobject, jstring);

}

#endif // RETIN_DISABLE_JAVA

#endif //__vlat__ProjectedPcaVlatBuilder_h__
