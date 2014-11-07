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
 * \file XMLInputSerializer.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/XMLInputSerializer.h"

#include <boost/format.hpp>

using namespace std;

namespace retin {

void	XMLInputSerializer::throwException (const std::string& func,const std::string& msg)
{
	throw std::runtime_error ((boost::format("Erreur décodage flux XML, ligne %2% (fonction %1%) :\n%3%")%func%line%msg).str());
}

void	XMLInputSerializer::skipSpaces ()
{
	while (s.peek() <= ' ')
	{
		int c = s.get();
		if (c == EOF)
			break;
		if (c == '\n') line ++;
	}
}

void	XMLInputSerializer::expect (const char c)
{
	//printf ("expect : %c\n",c);
	if (s.get() != c)
		throwException ("expect",(boost::format("Le symbole %c est attendu")%c).str());
}

void	XMLInputSerializer::expect (const char* pc)
{
	int i = 0;
	//printf ("expect : %s\n",pc);
	while (pc[i] != '\0')
	{
		int c = s.peek();
//			printf ("peek : %c\n",c);
		if (c == EOF)
			throwException ("expect","Terminaison anormale du flux");
		if (pc[i] != c)
			throwException ("expect","La chaîne "+std::string(pc)+" est attendue");
		s.get();
		if (c == '\n') line ++;
		i ++;
	}
}

bool	XMLInputSerializer::eof()
{
	skipSpaces ();
	if (s.peek() != '<')
		return false;
	s.get();
	if (s.peek() != '/')
	{
		s.unget();
		return false;
	}
	s.unget();
	return true;
}

const char* xmlHeader =	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n";
const char* xmlFooter = "</root>\n";

void	XMLInputSerializer::header ()
{
	skipSpaces ();
	expect (xmlHeader);
}

void	XMLInputSerializer::footer ()
{
	skipSpaces ();
	expect (xmlFooter);
}

}
