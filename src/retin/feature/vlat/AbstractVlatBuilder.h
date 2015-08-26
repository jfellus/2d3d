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

#ifndef __vlat__AbstractVlatBuilder_h__
#define __vlat__AbstractVlatBuilder_h__


#include "retin/toolbox/core/core.h"
#include "retin/toolbox/algebra/vector.h"
#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"
#include <vector>

namespace vlat {


class AbstractVlatBuilder
{
protected:
        size_t mainOrder,dictSize,centerDim;
        std::vector<float> centers;

virtual void    init() { }
virtual void    finit() { }

public:
        AbstractVlatBuilder(size_t mainOrder, size_t dictSize, size_t centerDim) :
            mainOrder(mainOrder), dictSize(dictSize), centerDim(centerDim),centers(dictSize*centerDim) {
        }
virtual size_t      getTensorDim(size_t cluster,size_t order) = 0;
virtual void        setCenter(size_t cluster, const float* center,size_t centerDim_) {
            if (centerDim != centerDim_)
                retinThrowException2("Invalid center dim %d != %d", centerDim_, centerDim);
            vector_cpy (&centers[cluster*centerDim],center,centerDim);
        }
virtual void        setCenters(const float* buffer, size_t centerCount, size_t centerDim) {
            if (centerCount != dictSize)
                retinThrowException2("Invalid center count %d != %d", centerCount, dictSize);
            size_t dim = 0;
            for (size_t cluster = 0; cluster < dictSize; cluster++) {
                setCenter(cluster,buffer + dim, centerDim);
                dim += centerDim;
            }
        }
virtual void        setMeanTensor(size_t cluster, size_t d, const float* meanTensor,size_t meanTensorDim) = 0;
virtual void        setMeanTensors(size_t d, const float* buffer,size_t meanTensorCount,size_t meanTensorDim) {
            if (meanTensorCount != dictSize)
                retinThrowException2("Invalid mean tensor count %d != %d", meanTensorCount, dictSize);
            size_t dim = 0;
            for (size_t cluster = 0; cluster < dictSize; cluster++) {
                setMeanTensor(cluster,d, buffer + dim, meanTensorDim);
                dim += getTensorDim(cluster,d);
            }
        }
virtual void process(const float* bag, size_t bagSize, size_t bagcenterDim) {

            if (bagcenterDim != centerDim)
                retinThrowException2("Wrong bag feature dim %d != %d", bagcenterDim, centerDim);

            init();
            for (size_t i = 0; i < bagSize; i++) {
                const float* data = bag + i*centerDim;
                size_t cMin = matrix_argmin_l2_float(data,&centers[0],centerDim,dictSize);
                add(cMin,data,centerDim);
            }

            finit();
        }
virtual void        add (size_t cluster,const float* data,size_t dim) = 0;
};


inline size_t  compTensorDim(size_t order,size_t centerDim) {
    switch (order) {
      case 1 : return centerDim;
      case 2 : return centerDim*centerDim;
    }
    retinThrowException1("Unsupported vlat order %d", order);
}

}

#endif
