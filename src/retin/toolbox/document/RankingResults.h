/**
 * \file RankingResults.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_RankingResults_h__
#define __retin_RankingResults_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/FloatMatrix.h"

#include <map>

namespace retin
{

class	RankingResults : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,RankingResults);

	std::map<std::string,FloatMatrixRef>	results;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, RankingResults& x)
	{
		s.object ("results",x.results);
	}

public:
};

typedef boost::shared_ptr<RankingResults> RankingResultsRef;

}

#endif

