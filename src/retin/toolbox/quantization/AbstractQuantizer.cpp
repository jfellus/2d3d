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
#include "AbstractQuantizer.h"

#include "retin/toolbox/algebra/vector.h"

#define QT_SHOW_CPUTIME

using namespace std;
using namespace retin;

namespace quantization {

AbstractQuantizer::AbstractQuantizer(size_t featureDim) : featureDim(featureDim),
        verboseLevel(0),maxWords(256),maxIterations(100),D(0),
        minDistortion(0),maxDistortion(1E-7),maxDistortionDiff(1E-4),
        workpool(NULL)
{
}

AbstractQuantizer::~AbstractQuantizer() {
}

double  AbstractQuantizer::computeDistortion() const {
    double D = 0;
    size_t n = getClusterCount();
    for (size_t c=0;c<n;c++)
        D += getAbstractCluster(c)->getDistortion();
    return D;
}

class TensorFunctor {
    AbstractCluster* cluster;
    float* buffer;
    size_t tensorDim,order;
public:
    TensorFunctor(AbstractCluster* cluster,float* buffer,size_t tensorDim,size_t order)
        : cluster(cluster),buffer(buffer),tensorDim(tensorDim),order(order) {        
    }
    void        operator()() {
        cluster->computeTensor(buffer,tensorDim,order);
    }
};

void    AbstractQuantizer::computeMeanTensors (float* buffer,size_t tensorDim,size_t order) {
    thread_pool_set workset(workpool);
    vector<AbstractCluster*> clusters = getAbstractClusters();
    for (size_t c = 0; c < clusters.size(); c++) {
    	TensorFunctor t(clusters[c], buffer+c*tensorDim, tensorDim, order);
        workset.push(t);
    }
    workset.join();
}


size_t AbstractQuantizer::getNonEmptyClusterCount() const {
    size_t count = 0;
    size_t n = getClusterCount();
    for (size_t c=0;c<n;c++)
        if (!getAbstractCluster(c)->isEmpty())
            count ++;
    return count;
}

double  AbstractQuantizer::getMeanDistortion() const {
    size_t n = getClusterCount();
    if (n > 0) {
        return getDistortion() / n;
    }
    return 1E300;
}


void    AbstractQuantizer::getCenter(size_t cluster,float* buf,size_t dim) const {
    const float* center = getAbstractCluster(cluster)->getCenter();
    vector_cpy (buf,center,dim);
}

void    AbstractQuantizer::getCenters(float* buf,size_t dim) const {
    for (size_t c=0;c<getClusterCount();c++) {
        getCenter(c,buf+c*dim,dim);
    }
}


void    AbstractQuantizer::showInfo(std::ostream& out) const
{
    std::vector<const AbstractCluster*> clusters = getAbstractClusters();
    out << clusters.size() << " clusters"
        << ", feature dim: " << featureDim
        << ", distortion: " << getDistortion() / sampleCount
        << ", mean distortion: " << getMeanDistortion() / sampleCount
        << endl;
    size_t count = 0;
    for (size_t c=0;c<clusters.size();c++) {
        const AbstractCluster& cluster = *clusters[c];
        if (verboseLevel >= 3) {
            out << "Cluster " << c << ": "
                << cluster.getSize() << " samples"
                << ", feature dim: " << cluster.getFeatureDim()
                << ", distortion: " << cluster.getDistortion()
                << endl;
        }
        count += cluster.getSize();
    }
    out << sampleCount << " samples (" << count << " in clusters)" << endl;
}


}
