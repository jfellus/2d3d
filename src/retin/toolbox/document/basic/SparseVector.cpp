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

#include "basic/SparseVector.h"
#include "../serialization/Serializers.h"

#include <map>

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(SparseVector);

using namespace retin;
using namespace std;

bool	SparseVector::equals(const Document* doc) const 
{
	if (!doc || !doc->isInstanceOf<SparseVector>())
		return false; 
	const SparseVector* x = static_cast<const SparseVector*>(doc);
	if (x->nDim != nDim || x->mData.size() != mData.size())
		return false;
        
        map<jint,jfloat>::const_iterator it1 = x->mData.begin();
        map<jint,jfloat>::const_iterator it2 = mData.begin();
        
        for(; it1!=x->mData.end() && it2!=mData.end() ; ++it1, ++it2){
            if(it1->first != it2->first)
                return false;
            if (fabs(it2->second - it1->second) > 1E-6)
		return false;
        }

	return true;
}
