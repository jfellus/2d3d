/**
 * \file Text.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Text_h__
#define __retin_Text_h__

#include "../Document.h"
#include "../../core/auto_array_ptr.h"

namespace retin
{

class	Text : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/basic,Text);

	std::string             strEncoding;
	jint			nSize;
	retin::auto_array_ptr<jbyte>	vData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Text& x)
	{
		s.object ("encoding",x.strEncoding);
		s.object ("size",x.nSize);
		s.vector ("chars",x.vData,x.nSize);
	}

public:
	typedef	uchar	value_type;

        Text() : strEncoding("UTF-8") { }

	const uchar*	data(int i=0) const { return (uchar*)vData.get(i); }
	uchar*		data(int i=0) { return (uchar*)vData.get(i); }
	jint		size() const { return nSize; }

	uchar*		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		return data(i);
	}
	const uchar*	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		return data(i);
	}
};

typedef boost::shared_ptr<Text> TextRef;

}

#endif

