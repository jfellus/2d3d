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

