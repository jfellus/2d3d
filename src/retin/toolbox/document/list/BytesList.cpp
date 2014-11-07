/**
 * \file BytesList.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "BytesList.h"

#include "../serialization/Serializers.h"

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(BytesList);

namespace retin {

void	BytesList::resize(int p,int n,bool bKeepData)
{
	if (nSize == n && nDim == p)
		return;
	if (!bKeepData) {
		nSize = n;
		nDim = p;
		size_t m = n;
		pData.resize (m*p); 
		return;
	}
	if (p != nDim) {
		retinThrowException("BytesList resizing unsupported if dim is changed");
	}
	else {
		jbyte* oldData = pData.release();
		size_t m = n;
		pData.resize (m*p);
		if (oldData) {
			m = std::min(n,nSize)*sizeof(jbyte);
			memcpy(pData.get(),oldData,m*p);
			delete[] oldData;
			nSize = n;
		}
	}
}

void	BytesList::show (int i0,int n) const
{
	std::cout << "Bytes List\n";
	for (size_t i=0;i<(size_t)n;i++) {
		for (size_t k=0;k<(size_t)nDim;k++)
			std::cout << (int)(pData[k+(i0+i)*nDim]) << " ";
		std::cout << std::endl;
	}
}


}

