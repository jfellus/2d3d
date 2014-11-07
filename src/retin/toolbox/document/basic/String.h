/**
 * \file String.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_String_h__
#define __retin_String_h__

#include "../Document.h"

namespace retin
{

class	String : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(java/lang,String);

	std::string	value;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, String& x)
	{
		s.object ("value",x.value);
	}

public:
	String () { }
	String (const char* value) : value(value) { }
	String (const std::string& value) : value(value) { }

	const std::string&	get() const { return value; }
	void			set(const std::string& x) { value = x; }
};


}

#endif

