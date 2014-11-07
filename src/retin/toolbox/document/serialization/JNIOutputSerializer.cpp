/**
 * \file JNIOutputSerializer.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/JNIOutputSerializer.h"

namespace retin {

JNIOutputSerializer::JNIOutputSerializer (JavaEnv& je,jobject obj) : je(je),obj(obj)
{ 
	objClass = je.getEnv()->GetObjectClass(obj);
	if (!objClass)
		throw std::runtime_error ("object() Pas de classe");
}

}

