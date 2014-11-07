/**
 * \file ImageFile.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_ImageFile_h__
#define __retin_ImageFile_h__

#include "retin/toolbox/document/FileName.h"

namespace retin
{

class	ImageFile : public FileName
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,ImageFile);

	jint nWidth,nHeight;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, ImageFile& x)
	{
		serialize(s,(FileName&)x);
		s.object ("width",x.nWidth);
		s.object ("height",x.nHeight);
	}

public:
	ImageFile () { }
	ImageFile (const std::string& fileName) : FileName(fileName) { }

};

typedef boost::shared_ptr<ImageFile>	ImageFileRef;

}

#endif

