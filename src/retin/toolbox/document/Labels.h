/**
 * \file Labels.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Labels_h__
#define __retin_Labels_h__

#include "retin/toolbox/document/GlobalLabel.h"

#include <vector>

namespace retin
{

class	Labels : public GlobalLabel
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,Labels);

	std::vector<DocumentRef>	labels;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Labels& x)
	{
		s.object ("labels",x.labels);
	}

public:
	std::vector<DocumentRef>&	getLabels() { return labels; }

	GlobalLabelRef		getGlobalLabel();

};

typedef boost::shared_ptr<Labels>	LabelsRef;

}

#endif

