/*
Copyright © CNRS 2012. 
Authors: David Picard, Philippe-Henri Gosselin, Romain Negrel, Hedi 
Tabia
Contact: picard@ensea.fr

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

*/
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
