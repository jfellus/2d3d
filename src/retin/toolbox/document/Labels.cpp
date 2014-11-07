/**
 * \file Labels.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/Labels.h"

#include "retin/toolbox/document/serialization/Serializers.h" 

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Labels);

namespace retin {

GlobalLabelRef	Labels::getGlobalLabel ()
{
	for (uint i=0;i<labels.size();i++)
		if ( labels[i]->isInstanceOf<GlobalLabel>() )
			return boost::static_pointer_cast<GlobalLabel> (labels[i]);
	return GlobalLabelRef();
}

}

