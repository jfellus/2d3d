/**
 * \file BinaryInputSerializer.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/BinaryInputSerializer.h"

#include <zlib.h>
#include <boost/format.hpp>

using namespace std;

namespace retin {

void	BinaryInputSerializer::throwException (const std::string& func,const std::string& msg)
{
	throw std::runtime_error ((boost::format("Erreur décodage flux Binaire, octet %2% (fonction %1%) :\n%3%")%func%s.tellg()%msg).str());
}

void	BinaryInputSerializer::readBuffer (char* buf,size_t n)
{
	char c = 0;
	s.read (&c,1);
	if (c == 'r')
	{
		size_t l = 0;
		s.read ((char*)&l,4);
		if (l != n)
			throwException ("readBuffer","Invalid size");
		s.read (buf,n);
	}
	else if (c == 'z')
	{
		uLong sourceLen = 0;
		s.read ((char*)&sourceLen,4);
		Bytef* source = new Bytef[sourceLen];
		s.read ((char*)source,sourceLen);
		size_t destLen = n;
		int ret = uncompress ((Bytef*)buf,&destLen,source,sourceLen);
		delete [] source;

		if (ret != Z_OK)
			throwException ("readBuffer","Error during Z uncompress");
		if (destLen != n)
			throwException ("readBuffer","Invalid size");
	}
	else
	{
		throwException ("readBuffer","Invalid code");
	}
}

void	BinaryInputSerializer::header ()
{
	char tmp[0x100];
	s.getline (tmp,0x100);
	if (strcmp(tmp,"<?bin version=\"1.0\" encoding=\"UTF-8\"?>") != 0)
		retinThrowException1("Invalid header %s",tmp);
}

void	BinaryInputSerializer::footer ()
{
}

}

