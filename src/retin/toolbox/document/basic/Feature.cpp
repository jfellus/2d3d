/**
 * \file Feature.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "basic/Feature.h"

#include "../serialization/Serializers.h"

#include <math.h>

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(Feature);


namespace retin {

bool	Feature::equals(const Document* doc) const 
{
	if (!doc || !doc->isInstanceOf<Feature>())
		return false; 
	const Feature* x = static_cast<const Feature*>(doc);
	if (x->nDim != nDim || x->vData.size() != vData.size())
		return false;
	for (uint i=0;i<vData.size();i++)
		if (fabs(vData[i]-x->vData[i]) > 1E-6)
			return false;
	return true;
}

void	Feature::show() const {
	std::cout << "Feature" << std::endl;
	for (int i=0;i<nDim;i++)
	    std::cout << vData[i] << " ";
	std::cout << std::endl;
}


}

