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
 * \file Filter.h
 * \author Romain Negrel
 * \version 4.0
 */

#ifndef __retin_Filter_h__
#define __retin_Filter_h__

#include "Document.h"
#include "../core/auto_array_ptr.h"

namespace retin
{

class	Filter : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,Filter);

	retin::auto_array_ptr<jfloat>	pData;
	jint				width,height;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Filter& x)
	{
		s.object ("width", x.width);
		s.object ("height", x.height);
		s.matrix ("data", x.pData, x.width, x.height);
	}

public:
	Filter (jfloat* pData,int width=1,int height=1,bool bOwnData=false) :
		pData(pData,width*height,bOwnData),width(width),height(height) { }
	Filter (int width=1,int height=1) : 
		pData(width*height),width(width),height(height) { }

	jfloat*		data() { return pData.get(); }
	void		setData (jfloat* p) { pData.set(p); }
        
	jint		getWidth() const { return width; }
	jint		getHeight() const { return height; }

	jfloat&		operator() (int i,int j=0)
	{
		BOOST_ASSERT (i >= 0 && i < width);
		BOOST_ASSERT (j >= 0 && j < height);
		return pData[i+j*width];
	}
	const jfloat&	operator() (int i,int j=0) const
	{
		BOOST_ASSERT (i >= 0 && i < width);
		BOOST_ASSERT (j >= 0 && j < height);
		return pData[i+j*width];
	}
};

typedef boost::shared_ptr<Filter> FilterRef;

}

#endif
