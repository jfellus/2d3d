/**
 * \file FileName.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_FileName_h__
#define __retin_FileName_h__

#include "retin/toolbox/document/Document.h"

namespace retin
{

class	FileName : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,FileName);

protected:
	std::string	value;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, FileName& x)
	{
		s.object ("file",x.value);
	}

public:
	FileName () { }
	FileName (const std::string& fileName) : value(fileName) { }

	const std::string&	get() const { return value; }
	void			set(const std::string& x) { value = x; }
};


}

#endif

