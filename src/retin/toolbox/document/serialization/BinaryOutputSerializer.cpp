/**
 * \file BinaryOutputSerializer.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/BinaryOutputSerializer.h"
#include <zlib.h>

using namespace std;

namespace retin {

void	BinaryOutputSerializer::writeBuffer (const char* buf,const size_t n)
{
	if (n >= 1024)
	{
		size_t destLen = (n*101)/100+12;
		Bytef* dest = new Bytef[destLen];
		int ret = compress (dest,&destLen,(const Bytef*)buf, n);
		if (ret == Z_OK && destLen+100 < n && (destLen*120)/100 < n)
		{
			char c = 'z';
			s.write (&c,1);
			s.write ((const char*)&destLen,4);
			s.write ((const char*)dest,destLen);
			delete [] dest;
			return;
		}
		delete [] dest;
	}

	char c = 'r';
	s.write (&c,1);
	s.write ((const char*)&n,4);
	s.write (buf,n);
}

void	BinaryOutputSerializer::header ()
{
	s << "<?bin version=\"1.0\" encoding=\"UTF-8\"?>\n";
}
void	BinaryOutputSerializer::footer ()
{
}


}

