/**
 * \file ArrayList.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_ArrayList_h__
#define __retin_ArrayList_h__

#include "../Document.h"

#include <vector>

namespace retin
{

class	ArrayList : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(java/util,ArrayList);

	std::vector<DocumentRef>	values;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, ArrayList& x)
	{
		s.object ("values",x.values);
	}

public:
	ArrayList () { }
	ArrayList (const std::vector<DocumentRef>& values) : values(values) { }

	int		size() const { return values.size(); }
	void		add (DocumentRef doc) { values.push_back(doc); }
	DocumentRef	get (int i) const { BOOST_ASSERT(i<(int)values.size()); return values[i]; }
	const std::vector<DocumentRef>&	getValues() const { return values; }

	template<class T>
	boost::shared_ptr<T> getAs (int i) const 
	{
                if (i < 0 || i >= (int)values.size())
                    retinThrowException1 ("Invalid index %d",i);
		DocumentRef doc = values[i];
                if (!doc)
                    retinThrowException1 ("Document %d is null",i);
		if (strcmp(doc->className(),T::staticClassName()) != 0)
			throw std::runtime_error ("L'objet n'est pas de type "+std::string(T::staticClassName()));
		return boost::static_pointer_cast<T> (doc);
	}
};

typedef boost::shared_ptr<ArrayList>	ArrayListRef;

}

#endif

