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
