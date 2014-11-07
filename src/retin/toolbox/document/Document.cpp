/**
 * \file Document.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/serialization/Java.h"

using namespace retin;

JNIEXPORT jobject JNICALL Java_retin_toolbox_document_Document_toXMLString
  (JNIEnv * env, jobject thisObject)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	return env->NewStringUTF ( je.createDocument<Document>(thisObject)->toXMLString().c_str() );

	RETIN_JAVA_METHOD_END
	return NULL;
}

JNIEXPORT jobject JNICALL Java_retin_toolbox_document_Document_makeXMLString
  (JNIEnv * env, jobject, jobject object)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	return env->NewStringUTF ( je.createDocument<Document>(object)->toXMLString().c_str() );

	RETIN_JAVA_METHOD_END
	return NULL;
}


JNIEXPORT void JNICALL Java_retin_toolbox_document_Document_fromXMLString
  (JNIEnv * env, jobject thisObject, jobject object)
{
	RETIN_JAVA_METHOD_BEGIN

/*	JavaEnv je(env);
	je.createString(object);
	return env->NewStringUTF ( je.createObject<Document>(thisObject)->toXMLString().c_str() );*/

	RETIN_JAVA_METHOD_END
}


