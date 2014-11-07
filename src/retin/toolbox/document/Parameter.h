/**
 * \file Parameter.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Parameter_h__
#define __retin_Parameter_h__

#include "retin/toolbox/document/Document.h"

namespace retin
{

class	Parameters;

class	Parameter : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,Parameter);

friend  class	Parameters;

	std::string			value;
	boost::shared_ptr<Parameters>	params;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Parameter& x)
	{
		s.object ("value",x.value);
		s.object ("params",x.params);
	}
public:
	Parameter () { }
	Parameter (const std::string& value) : value(value) { }
	Parameter (const Parameter& param);
	Parameter (const std::string& value,boost::shared_ptr<Parameters> params) : value(value),params(params) { }

	virtual bool	equals(const Document* doc) const;
};

typedef boost::shared_ptr<Parameter> ParameterRef;

}

#endif

