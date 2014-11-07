/**
 * \file LogResults.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_LogResults_h__
#define __retin_LogResults_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/LogResult.h"

#include <map>

namespace retin
{

class	LogResults : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,LogResults);

	std::map<std::string,LogResultRef>	categories;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, LogResults& x)
	{
		s.object ("categories",x.categories);
	}

public:
};

typedef boost::shared_ptr<LogResults> LogResultsRef;

}

#endif

