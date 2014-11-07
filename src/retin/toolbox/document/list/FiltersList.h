/**
 * \file FiltersList.h
 * \author Romain Negrel
 * \version 4.0
 */

#ifndef __retin_Filters_List_h__
#define __retin_Filters_List_h__

#include "../Document.h"
#include "../../core/auto_array_ptr.h"

namespace retin
{

class	FiltersList : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,FiltersList);

	retin::auto_array_ptr<jfloat>	pData;
	jint				filtersCount,width,height;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, FiltersList& x)
	{
		s.object ("width", x.width);
		s.object ("height", x.height);
                s.object ("filtersCount", x.filtersCount);
		s.matrix ("data", x.pData, x.width * x.height, x.filtersCount);
	}

public:
	FiltersList (jfloat* pData,int width=1,int height=1,bool bOwnData=false) :
		pData(pData,width*height*filtersCount,bOwnData),filtersCount(filtersCount),width(width),height(height) { }
	FiltersList (int width=1,int height=1) : 
		pData(width*height*filtersCount),filtersCount(filtersCount),width(width),height(height) { }

	jfloat*		data() { return pData.get(); }
	void		setData (jfloat* p) { pData.set(p); }
        
	jint		getWidth() const { return width; }
	jint		getHeight() const { return height; }
        jint		getFiltersCount() const { return filtersCount; }

	jfloat&		operator() (int id, int i,int j=0)
	{
                BOOST_ASSERT (id >= 0 && id < filtersCount);
		BOOST_ASSERT (i >= 0 && i < width);
		BOOST_ASSERT (j >= 0 && j < height);
		return pData[i+j*width+filtersCount*width*height];
	}
	const jfloat&	operator() (int id, int i,int j=0) const
	{
                BOOST_ASSERT (id >= 0 && id < filtersCount);
		BOOST_ASSERT (i >= 0 && i < width);
		BOOST_ASSERT (j >= 0 && j < height);
		return pData[i+j*width+filtersCount*width*height];
	}
};

typedef boost::shared_ptr<FiltersList> FiltersListRef;

}

#endif

