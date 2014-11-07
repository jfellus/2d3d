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

