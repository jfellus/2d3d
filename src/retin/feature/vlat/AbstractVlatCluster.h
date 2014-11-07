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

#ifndef __vlat__VlatCluster_h__
#define __vlat__VlatCluster_h__

#include "retin/toolbox/core/core.h"

namespace vlat {

class AbstractVlatCluster
{
protected:
        size_t      mainOrder;
        size_t      tensorDims[2];
        const float* meanTensors[2];
public:
        AbstractVlatCluster() : mainOrder(0) {
            for (size_t d=0;d<2;d++) {
                tensorDims[d] = 0;
                meanTensors[d] = NULL;
            }
        }
virtual ~AbstractVlatCluster() {}

        size_t  getMainOrder() const {
            return mainOrder;
        }
        void    setMainOrder(size_t dim) {
            mainOrder = dim;
        }
        void    setTensorDims(size_t dim) {
            size_t x = dim;
            for (size_t d=0;d<2;d++) {
                tensorDims[d] = x;
                x *= dim;
            }
        }
        size_t  getTensorDim(size_t order) const {
            if (order <= 0 || order > 2)
                retinThrowException1("Unsupporder order %d",order);
            return tensorDims[order-1];
        }
        const float*    getMeanTensor(size_t order) const {
            if (order <= 0 || order > 2)
                retinThrowException1("Unsupporder order %d",order);
            return meanTensors[order-1];
        }
        void    setMeanTensor(size_t order,const float* meanTensor,size_t meanTensorDim) {
            if (order <= 0 || order > 2)
                retinThrowException1("Unsupporder order %d",order);
            if (meanTensorDim != tensorDims[order-1])
                retinThrowException2("Invalid tensor dim %d != %d",meanTensorDim,tensorDims[order-1]);
            if (!vector_isfinite_float(meanTensor,meanTensorDim))
                retinThrowException("Tensor is not finite");
            meanTensors[order-1] = meanTensor;
        }

virtual void    init() { }
virtual void    add(const float* feature,size_t featureDim) = 0;

};



}

#endif
