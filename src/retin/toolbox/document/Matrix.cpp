/**
 * \file Matrix.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/Matrix.h"

#include "retin/toolbox/document/serialization/Serializers.h" 

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Matrix);

namespace retin {

void	Matrix::resize(int nVectorSize_,int nVectorCount_) {
    nVectorSize = nVectorSize_;
    nVectorCount = nVectorCount_;
    pData.resize(nVectorSize*nVectorCount);
    memset(pData.get(),0,nVectorSize*nVectorCount*sizeof(double));
}

}

