/**
 * \file Serializable.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/Serializable.h"

#include "retin/toolbox/document/serialization/Serializers.h" 

#include <sstream>

namespace retin {
 
std::string	Serializable::toXMLString ()
{
	std::ostringstream os;
	XMLOutputSerializer osx(os);
	serializeOut (osx);
	return os.str();
}

void	Serializable::fromXMLString (const std::string& str)
{
	std::istringstream is(str);
	XMLInputSerializer isx(is);
	serializeIn (isx);
}

void	Serializable::toJavaObject (JavaEnv& je,jobject obj)
{
	JNIOutputSerializer jos(je,obj);
	serializeOut (jos);	 
}

void	Serializable::fromJavaObject (JavaEnv& je,jobject obj)
{
	JNIInputSerializer jis(je,obj);
	serializeIn (jis);
}

}
