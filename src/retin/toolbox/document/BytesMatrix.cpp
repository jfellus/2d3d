/**
 * \file BytesMatrix.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/BytesMatrix.h"

#include "retin/toolbox/document/serialization/Serializers.h" 

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(BytesMatrix);

namespace retin {

bool	BytesMatrix::equals(const Document* doc) const 
{
	if (!doc || !doc->isInstanceOf<BytesMatrix>())
		return false; 
	const BytesMatrix* x = static_cast<const BytesMatrix*>(doc);
	if (x->nDim != nDim || x->nWidth != nWidth || x->nHeight != nHeight || x->vData.size() != vData.size())
		return false;
	for (uint i=0;i<vData.size();i++)
		if (vData[i] != x->vData[i])
			return false;
	return true;
}


}

