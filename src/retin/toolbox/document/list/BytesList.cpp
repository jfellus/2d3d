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
