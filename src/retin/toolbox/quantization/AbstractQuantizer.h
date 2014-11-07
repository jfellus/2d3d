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

#ifndef __quantization__AbstractQuantizer_h__
#define __quantization__AbstractQuantizer_h__

#include "AbstractCluster.h"
#include "retin/toolbox/core/thread_pool.h"


namespace quantization {

class AbstractQuantizer
{
protected:
    const float*    mainData;
    size_t          featureDim;
    size_t          sampleCount;
    size_t          verboseLevel;
    size_t          maxWords;
    size_t          maxIterations;
    double          D;
    double          minDistortion;
    double          maxDistortion;
    double          maxDistortionDiff;
    thread_pool* workpool;
    std::vector<int> labels;

public:
    AbstractQuantizer(size_t featureDim);
    virtual ~AbstractQuantizer();

    void            setVerboseLevel (size_t x) { verboseLevel = x; }
    void            setMaxWords (size_t x) { maxWords = x; }
    void            setMaxIterations (size_t x) { maxIterations = x; }
    void            setMinDistortion (double x) { minDistortion = x; }
    void            setMaxDistortion (double x) { maxDistortion = x; }
    void            setMaxDistortionDiff (double x) { maxDistortionDiff = x; }
    void            setWorkPool(thread_pool* wp) { workpool = wp; }

    size_t          getFeatureDim() const { return featureDim; }
    size_t          getVerboseLevel () const { return verboseLevel; }
    size_t          getMaxWords () const { return maxWords; }
    size_t          getMaxIterations () const { return maxIterations; }
    double          getMinDistortion () const { return minDistortion; }
    double          getMaxDistortion () const { return maxDistortion; }
    double          getMaxDistortionDiff () const { return maxDistortionDiff; }
    double          computeDistortion() const;
    void            computeMeanTensors (float* buffer,size_t tensorDim,size_t order);
    std::vector<int> getLabels() const { return labels; }
    virtual double  getDistortion() const { return D; }
    virtual double  getMeanDistortion() const;
    virtual void    getCenter(size_t cluster,float* buf,size_t dim) const;
    virtual void    getCenters(float* buf,size_t dim) const;

    virtual void    run(const float* data,size_t dataDim,size_t dataCount) = 0;



    virtual size_t  getClusterCount() const = 0;
    virtual size_t  getNonEmptyClusterCount() const;
    virtual const AbstractCluster*          getAbstractCluster(size_t cluster) const = 0;
    virtual AbstractCluster*                getAbstractCluster(size_t cluster) = 0;
    virtual std::vector<const AbstractCluster*>   getAbstractClusters() const {
       std::vector<const AbstractCluster*> clusters;
       for (size_t c=0;c<getClusterCount();c++)
            clusters.push_back(getAbstractCluster(c));
       return clusters;
    }
    virtual std::vector<AbstractCluster*>   getAbstractClusters() {
       std::vector<AbstractCluster*> clusters;
       for (size_t c=0;c<getClusterCount();c++)
            clusters.push_back(getAbstractCluster(c));
       return clusters;
    }

    size_t          getLabel(size_t i) const { return labels[i]; }
    void            setLabel(size_t i,size_t c) { labels[i] = c; }
    const float*    getSample(size_t i) const { return mainData+i*featureDim; }

    void            check() const;
    void            showInfo(std::ostream&) const;

};

}

#endif
