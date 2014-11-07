/**
 * \file VideoFile.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_VideoFile_h__
#define __retin_VideoFile_h__

#include "retin/toolbox/document/FileName.h"

namespace retin
{

class	VideoFile : public FileName
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,VideoFile);

	jint 	nWidth,nHeight;
	jdouble	fFps;
	jlong	nStart,nDuration;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, VideoFile& x)
	{
		serialize(s,(FileName&)x);
		s.object ("width",x.nWidth);
		s.object ("height",x.nHeight);
		s.object ("fps",x.fFps);
		s.object ("start",x.nStart);
		s.object ("duration",x.nDuration);
	}

public:
	VideoFile () { }
	VideoFile (const std::string& fileName) : FileName(fileName),fFps(25) { }

};

typedef boost::shared_ptr<VideoFile>	VideoFileRef;
}

#endif

