/**
 * \file MeshFile.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_MeshFile_h__
#define __retin_MeshFile_h__

#include "retin/toolbox/document/FileName.h"

namespace retin
{

class	MeshFile : public FileName
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,MeshFile);

	jint nNodeCount,nEdgeCount;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, MeshFile& x)
	{
		serialize(s,(FileName&)x);
		s.object ("nodeCount",x.nNodeCount);
		s.object ("edgeCount",x.nEdgeCount);
	}

public:
	MeshFile () { }
	MeshFile (const std::string& fileName) : FileName(fileName) { }

};

typedef boost::shared_ptr<MeshFile>	MeshFileRef;

}

#endif

