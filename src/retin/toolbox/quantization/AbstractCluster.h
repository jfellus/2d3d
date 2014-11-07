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

#ifndef __quantization__AbstractCluster_h__
#define __quantization__AbstractCluster_h__

#include "retin/toolbox/core/core.h"
#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"

#include <vector>

#ifdef RETIN_ENABLE_BOOST
#include <boost/thread.hpp>
#endif

namespace quantization {

class AbstractCluster {
protected:
    const float*            mainData;
    size_t                  featureDim;
    std::vector<float>      center;
    double                  distortion;
    std::vector<size_t>     samples;
    bool                    split,fusion;

    #ifdef RETIN_ENABLE_BOOST
    mutable boost::mutex    the_mutex;
    #endif

public:
    AbstractCluster(const float* mainData,size_t featureDim);
    virtual ~AbstractCluster();

    double          getDistortion() const { return distortion; }
    size_t          getFeatureDim() const { return featureDim; }
    size_t          getSize() const { return samples.size(); }
    const float*    getMainData() const { return mainData; }
    bool            isEmpty() const { return samples.size() == 0; }
    const float*    getCenter() const { return &center[0]; }
    float*          getCenter() { return &center[0]; }
    void            setCenter(const float* buf,size_t dim);
    bool            isSplit() const { return split; }
    bool            isFusion() const { return fusion; }
    void            setSplit(bool b) { split = b; }
    void            setFusion(bool b) { fusion = b; }

    virtual void    setSamples(size_t dataCount);
    virtual void    computeCenter();
    virtual void    computeDistortion();
    virtual void    computeTensor(float* buf,size_t bufSize,int order) const;

    virtual void    updateLabels (std::vector<int>& labels,size_t cluster) const;
    virtual bool    splitCluster(AbstractCluster& cluster1,AbstractCluster& cluster2) const;
    virtual bool    mergeClusters(const AbstractCluster& cluster1,const AbstractCluster& cluster2);


    const float*    getSample(size_t i) const { return mainData+i*featureDim; }
    size_t			getSampleIdx(size_t i) const { return samples[i]; }
    const float*    getClusterSample(size_t i) const { return getSample(samples[i]); }
    float           distTo(const float* data) const { return vector_l2p2_float(&center[0],data,featureDim); }
    float           distTo(size_t i) const { return distTo(getSample(i)); }

    void            clear() {
        samples.clear();
        distortion = 0;
    }
    void            addSample(size_t i,double sampleDist) {
        #ifdef RETIN_ENABLE_BOOST
        boost::mutex::scoped_lock lock(the_mutex);
        #endif
        samples.push_back(i);
        distortion += sampleDist;
    }
};

}

#endif
