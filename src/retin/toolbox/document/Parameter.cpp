/**
 * \file Parameter.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/Parameter.h"
#include "retin/toolbox/document/Parameters.h"

#include "retin/toolbox/document/serialization/Serializers.h" 

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Parameter);

namespace retin  {

Parameter::Parameter (const Parameter& p) : value(p.value) {
	if (p.params)
	    params = boost::make_shared<Parameters>(*p.params);
}

bool	Parameter::equals(const Document* doc) const 
{
	if (!doc || !doc->isInstanceOf<Parameter>())
		return false; 
	const Parameter* x = static_cast<const Parameter*>(doc);
	if (x->value != value)
		return false;
	if (x->params) {
		if (!params)
			return false;
		return x->params->equals(params.get());
	}
	return !params;
}

}