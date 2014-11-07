/*
Copyright © CNRS 2012. 
Authors: David Picard, Philippe-Henri Gosselin, Romain Negrel, Hedi 
Tabia
Contact: picard@ensea.fr

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

*/
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
