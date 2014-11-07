/**
 * \file KeypointList.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "KeypointList.h"

#include "../serialization/Serializers.h"

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(KeypointList);

using namespace retin;
using namespace std;

namespace retin {

KeypointList::KeypointList (uint nCount,uint nDim) : nCount(nCount)
{
	for (uint i=0;i<nCount;i++)
		pKeypoints.push_back(boost::make_shared<Keypoint>(nDim));
}

}
