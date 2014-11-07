/**
 * \file IntegralImage.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_IntegralImage_h__
#define __retin_IntegralImage_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/BoxLabel.h"

namespace retin
{

class	IntegralImage : public Document
{
public:

virtual	uint	dim() const = 0;
virtual	uint	width() const = 0;
virtual	uint	height() const = 0;
virtual	float	value(int i,int j,int k) const = 0;
virtual	float	getCount (int i1,int j1,int n,int m,int k) const {
	    float result = value(i1+n,j1+m,k);
	    result -= value(i1,j1+m,k);
	    result -= value(i1+n,j1,k);
	    result += value(i1,j1,k);
	    return result;
	}
virtual	float	getCount (float i1,float j1,float n,float m,int k) const {
	    return getCount((int)(i1*width()),(int)(j1*height()),(int)(n*width()),(int)(m*height()),k);
	}
virtual	float	searchCount (int width,int height,int bin) const = 0;
virtual	void	searchBox (BoxLabel& box,int width,int height,int bin) const = 0;
virtual bool	searchBox (BoxLabel& box,int width,int height,int k,int x1,int y1,int x2,int y2) const = 0;



};

typedef boost::shared_ptr<IntegralImage>	IntegralImageRef;

}



#endif

