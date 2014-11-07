/**
 * \file Integer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Integer_h__
#define __retin_Integer_h__

#include "../Document.h"

namespace retin
{

class	Integer : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(java/lang,Integer);

	jint		value;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Integer& x)
	{
		s.object ("value",x.value);
	}

public:
	Integer () : value(0) { }
	Integer (jint value) : value(value) { }

	virtual bool	equals(const Document* doc) const;
	jint		get() const { return value; }
	void		set(jint x) { value = x; }
};

typedef boost::shared_ptr<Integer> IntegerRef;

}

#endif

