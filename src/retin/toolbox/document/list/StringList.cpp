/**
 * \file StringList.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "StringList.h"

#include "../serialization/Serializers.h"

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(StringList);

namespace retin {

bool	StringList::equals(const Document* doc) const 
{
	if (!doc || !doc->isInstanceOf<StringList>())
		return false; 
	const StringList* list = static_cast<const StringList*>(doc);
	if (list->vData.size() != vData.size())
		return false;
	for (uint i=0;i<vData.size();i++)
		if (vData[i] != list->vData[i])
			return false;
	return true;
}


}