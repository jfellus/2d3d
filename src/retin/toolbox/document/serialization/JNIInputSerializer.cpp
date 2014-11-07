/**
 * \file JNIInputSerializer.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/JNIInputSerializer.h"


namespace retin {

JNIInputSerializer::JNIInputSerializer (JavaEnv& je,jobject obj) : je(je),obj(obj)
{ 
	objClass = je.getEnv()->GetObjectClass(obj);
	if (!objClass)
		throw std::runtime_error ("Pas de classe");
}

}

