/**
 * \file BoxLabel.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_BoxLabel_h__
#define __retin_BoxLabel_h__

#include "retin/toolbox/document/GlobalLabel.h"

namespace retin
{

class	BoxLabel : public GlobalLabel
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,BoxLabel);

	jfloat		x1,y1,x2,y2;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, BoxLabel& x)
	{
		serialize (s,(GlobalLabel&)x);
	       	s.object ("x1",x.x1);
		s.object ("y1",x.y1);
		s.object ("x2",x.x2);
		s.object ("y2",x.y2);
	}

public:
	BoxLabel (float x1=0,float y1=0,float x2=1,float y2=1,int label=0,float weight=1) : GlobalLabel(label,weight),x1(x1),y1(y1),x2(x2),y2(y2) { }

	jfloat		getX1() const { return x1; }
	jfloat		getY1() const { return y1; }
	jfloat		getX2() const { return x2; }
	jfloat		getY2() const { return y2; }

	jfloat		width() const { return x2-x1; }
	jfloat		height() const { return y2-y1; }

	void		setX1 (jfloat x) { x1 = x; }
	void		setY1 (jfloat x) { y1 = x; }
	void		setX2 (jfloat x) { x2 = x; }
	void		setY2 (jfloat x) { y2 = x; }

	void		div (jfloat width,jfloat height) {
		x1 /= width; y1 /= height;
		x2 /= width; y2 /= height;
	}

	void		draw(float* image,size_t width,size_t height) const;
	void		show() const;


};

typedef boost::shared_ptr<BoxLabel>	BoxLabelRef;

}

#endif

