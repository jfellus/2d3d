/**
 * \file Float.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Float_h__
#define __retin_Float_h__

#include "../Document.h"

namespace retin
{

class	Float : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(java/lang,Float);

	jfloat		value;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Float& x)
	{
		s.object ("value",x.value);
	}

public:
	Float () : value(0) { }
	Float (jfloat value) : value(value) { }

	jfloat		get() const { return value; }
	void		set(jfloat x) { value = x; }
};


}

#endif

