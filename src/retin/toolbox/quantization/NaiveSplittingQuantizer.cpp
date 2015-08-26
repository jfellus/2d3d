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
#include "NaiveSplittingQuantizer.h"

#include "retin/toolbox/core/string_retin.h"

#include <memory>
#include <math.h>

using namespace std;
using namespace retin;

namespace quantization {

void    NaiveSplittingQuantizer::clear() {
    for (size_t i=0;i<clusters.size();i++)
        if (clusters[i])
            delete clusters[i];
    clusters.clear();
    labels.clear();
    D = 0;
}

void    NaiveSplittingQuantizer::run(const float* data,size_t dataDim,size_t dataCount)
{
    if (dataDim != featureDim)
        retinThrowException2("Invalid dim %d != %d",dataDim,featureDim);
    clear();
    labels.resize(dataCount);
    mainData = data;
    sampleCount = dataCount;
    minDistortion *= sampleCount;
    maxDistortion *= sampleCount;
    
    
    // Initialization sur le centre de gravité
    clusters.clear();
    auto_ptr<NaiveSplittingCluster> cluster0 (new NaiveSplittingCluster(mainData,featureDim));
    cluster0->setSamples(sampleCount);
    cluster0->computeCenter();
    cluster0->computeDistortion();
    D = cluster0->getDistortion();
    clusters.push_back(cluster0.release());

    size_t prevClusterCount = 0;
    while (true) {

        if (verboseLevel >= 2 && clusters.size() >= 2*prevClusterCount) {
            std::cout << "Init " << getClusterCount() << " clusters"
                      << ", distortion = " << D / sampleCount // << " (=" << computeDistortion()/sampleCount << ")"
                      << ", mean distortion = " << getMeanDistortion() / sampleCount
                      << std::endl;
            prevClusterCount = clusters.size();
        }
        if (D <= minDistortion || clusters.size() >= maxWords)
            break;

        // Recherche le cluster avec la + forte distortion
        size_t cMax = ~0;
        NaiveSplittingCluster* clusterMax = NULL;
        for (size_t c=0;c<clusters.size();c++) {
            if (clusters[c]->getSize() > 1 && (!clusterMax || clusters[c]->getDistortion() > clusterMax->getDistortion())) {
                clusterMax = clusters[c];
                cMax = c;
            }
        }
        if (!clusterMax) {
            if (verboseLevel >= 2) {
                std::cout << "No cluster with more than one element" << std::endl;
            }
            break;
        }

        auto_ptr<NaiveSplittingCluster> cluster1 (new NaiveSplittingCluster(mainData,featureDim));
        auto_ptr<NaiveSplittingCluster> cluster2 (new NaiveSplittingCluster(mainData,featureDim));
        if (!clusterMax->splitCluster (*cluster1,*cluster2)) {
             if (verboseLevel >= 2) {
                std::cout << "Empty clusters" << std::endl;
            }
            break;
        }

        if (verboseLevel >= 4) {
            std::cout << string_format("Split %f => %f + %f, %d = %d + %d",
             clusters[cMax]->getDistortion(),cluster1->getDistortion(),cluster2->getDistortion(),
             clusters[cMax]->getSize(),cluster1->getSize(),cluster2->getSize())
                << std::endl;
        }
        D += cluster1->getDistortion()+cluster2->getDistortion()-clusterMax->getDistortion();

        cluster1->updateLabels(labels,cMax);
        setCluster(cMax,cluster1.release());

        cluster2->updateLabels(labels,clusters.size());
        clusters.push_back(cluster2.release());

    }
}

}
