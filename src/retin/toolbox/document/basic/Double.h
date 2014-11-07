/**
 * \file Double.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Double_h__
#define __retin_Double_h__

#include "../Document.h"

namespace retin
{

class	Double : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(java/lang,Double);

	jdouble		value;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Double& x)
	{
		s.object ("value",x.value);
	}

public:
	Double () : value(0) { }
	Double (jdouble value) : value(value) { }

	jdouble		get() const { return value; }
	void		set(jdouble x) { value = x; }
};


}

#endif

