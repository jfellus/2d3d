/**
 * \file StringList.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_StringList_h__
#define __retin_StringList_h__

#include "../Document.h"

#include <vector>

namespace retin
{

class	StringList : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/list,StringList);

	std::vector<std::string> vData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, StringList& x)
	{
		s.object ("strings",x.vData);
	}

public:

	jint		size() const { return vData.size(); }

	void		add(const std::string& str) { vData.push_back(str); }

	virtual bool	equals(const Document* doc) const;

	std::string&		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < (int)vData.size());
		return vData[i];
	}
	const std::string&	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < (int)vData.size());
		return vData[i];
	}
};

typedef boost::shared_ptr<StringList> StringListRef;

}

#endif

