/**
 * \file LogResult.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_LogResult_h__
#define __retin_LogResult_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/FloatMatrix.h"

#include <map>

namespace retin
{

class	LogResult : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,LogResult);

	std::map<std::string,FloatMatrixRef>	vars;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, LogResult& x)
	{
		s.object ("vars",x.vars);
	}

public:
};

typedef boost::shared_ptr<LogResult> LogResultRef;

}

#endif

