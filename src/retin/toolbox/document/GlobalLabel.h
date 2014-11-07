/**
 * \file GlobalLabel.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_GlobalLabel_h__
#define __retin_GlobalLabel_h__

#include "retin/toolbox/document/Document.h"

namespace retin
{

class	GlobalLabel : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,GlobalLabel);

	jint		label;
	jfloat		weight;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, GlobalLabel& x)
	{
		s.object ("label",x.label);
		s.object ("weight",x.weight);
	}

public:
	GlobalLabel (int label=0,float weight=1) : label(label),weight(weight) { }

	jint	    getLabel() const { return label; }
	jfloat	    getWeight() const { return weight; }

	void	    setLabel (jint x) { label = x; }
	void	    setWeight (jfloat x) { weight = x; }

};

typedef boost::shared_ptr<GlobalLabel>	GlobalLabelRef;

}

#endif

