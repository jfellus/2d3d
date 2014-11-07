/**
 * \file TreeMap.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_TreeMap_h__
#define __retin_TreeMap_h__

#include "retin/toolbox/document/Document.h"
#include <map>

namespace retin
{

class	TreeMap : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(java/util,TreeMap);

	std::map<std::string,DocumentRef>	values;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, TreeMap& x)
	{
		s.object ("values",x.values);
	}

public:

	std::map<std::string,DocumentRef>&	getValues() { return values; }

	bool		has (const std::string& x) const { return values.find(x) != values.end(); }
	void		set (const std::string& x,DocumentRef doc) { values[x] = doc; }
	DocumentRef	get (const std::string& x) const {
            std::map<std::string,DocumentRef>::const_iterator ite = values.find(x);
            if (ite == values.end())
                retinThrowException1("No key %s in TreeMap",x.c_str());
            return ite->second;
        }

	typedef	std::map<std::string,DocumentRef>::iterator	iterator;
	iterator	begin() { return values.begin(); }
	iterator	end() { return values.end(); }
};

typedef boost::shared_ptr<TreeMap> TreeMapRef;

}

#endif

