
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

