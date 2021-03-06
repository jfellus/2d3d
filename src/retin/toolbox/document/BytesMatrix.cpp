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
