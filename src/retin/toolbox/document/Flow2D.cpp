/**
 * \file Flow2D.cpp
 * \author Romain Negrel
 * \version 4.0
 */

#include "retin/toolbox/document/Flow2D.h"

#include "retin/toolbox/document/serialization/Serializers.h" 

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Flow2D);

namespace retin {

void	Flow2D::resize(int width_,int height_) {
    nWidth = width_;
    nHeight = height_;
    pFlowX.resize(nWidth*nHeight);
    pFlowY.resize(nWidth*nHeight);
    memset(pFlowX.get(), 0, nWidth*nHeight*sizeof(float));
    memset(pFlowY.get(), 0, nWidth*nHeight*sizeof(float));
}

}
