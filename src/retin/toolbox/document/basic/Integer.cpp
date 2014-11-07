/**
 * \file Integer.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "basic/Integer.h"

#include "../serialization/Serializers.h"

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Integer);

namespace retin {

bool	Integer::equals(const Document* doc) const 
{
	if (!doc || !doc->isInstanceOf<Integer>())
		return false; 
	return static_cast<const Integer*>(doc)->value == value;
}

}