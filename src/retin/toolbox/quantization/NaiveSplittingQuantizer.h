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

#ifndef __quantization__NaiveSplittingQuantizer_h__
#define __quantization__NaiveSplittingQuantizer_h__

#include "AbstractQuantizer.h"

namespace quantization {

class NaiveSplittingCluster : public AbstractCluster {
protected:

public:
    NaiveSplittingCluster(const float* mainData,size_t featureDim) : AbstractCluster(mainData,featureDim) {}
};


class NaiveSplittingQuantizer : public AbstractQuantizer
{
protected:
    std::vector<NaiveSplittingCluster*> clusters;

    void    setCluster(size_t c,NaiveSplittingCluster* cluster) {
         if (c < 0 || c >= clusters.size())
            retinThrowException2("Invalid cluster %d not in [0,%d[",c,clusters.size());
        if (clusters[c]) delete clusters[c];
        clusters[c] = cluster;
    }
    
public:
    NaiveSplittingQuantizer(size_t featureDim) : AbstractQuantizer(featureDim) { }
    virtual ~NaiveSplittingQuantizer() { clear(); }

    virtual void    clear();
    virtual void    run(const float* data,size_t dataDim,size_t dataCount);

    virtual size_t  getClusterCount() const { return clusters.size(); }
    virtual const AbstractCluster* getAbstractCluster(size_t c) const {
         if (c < 0 || c >= clusters.size())
            retinThrowException2("Invalid cluster %d not in [0,%d[",c,clusters.size());
         return clusters[c];
    }
    virtual AbstractCluster* getAbstractCluster(size_t c) {
         if (c < 0 || c >= clusters.size())
            retinThrowException2("Invalid cluster %d not in [0,%d[",c,clusters.size());
         return clusters[c];
    }



};

}

#endif
