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
 * \file Java.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/Java.h"

#include "retin/toolbox/document/serialization/JNIInputSerializer.h"
#include "retin/toolbox/document/serialization/JNIOutputSerializer.h"

#include "retin/toolbox/document/basic/Integer.h"
#include "retin/toolbox/document/basic/Float.h"
#include "retin/toolbox/document/basic/Double.h"
#include "retin/toolbox/document/basic/String.h"

#include "retin/toolbox/document/list/ArrayList.h"

#include <boost/format.hpp>
#include <fstream>

using namespace std;

namespace retin {

class JavaObjectFactory : public ObjectFactory<Serializable> {
public:
        template<class T>
        void registerClass() {
            ObjectFactory<Serializable>::registerClass<T > (T::staticClassName());
        }

};


JavaEnv::JavaEnv (JNIEnv* env,bool bAllowPrimitiveArrayCritical) : env(env),bArrayCopy(true),bAllowPrimitiveArrayCritical(bAllowPrimitiveArrayCritical)
{
	factoryRef = getDefaultSerializableObjectFactory();
}

JavaEnv::~JavaEnv ()
{
	unregisterArrayPtrs ();
	unregisterReferences ();
}

void	JavaEnv::unregisterArrayPtrs()
{
	for (uint i=0;i<arrayPtrs.size();i++)
	{
		JavaArrayPtr* p = arrayPtrs[i];
		p->release (env);
		delete p;
	}
	arrayPtrs.clear ();
}

jobject JavaEnv::registerReference (jobject object)
{
	jobject o = env->NewGlobalRef(object);
	globalRefs.push_back(o);
//		cout << "Register " << (size_t)o << endl;
	return o;
}

void	JavaEnv::unregisterReferences()
{
	for (uint i=0;i<globalRefs.size();i++)
	{
		env->DeleteGlobalRef (globalRefs[i]);
//		cout << "Release " << (size_t)globalRefs[i] << endl;
	}
	globalRefs.clear ();
}


std::string	JavaEnv::getClassName (jclass classObj)
{
	jstring jclassName = (jstring)callMethod<jobject> (classObj,"getName","()Ljava/lang/String;");
	const char* buf = env->GetStringUTFChars(jclassName,NULL);
	std::string className(buf);
	env->ReleaseStringUTFChars(jclassName,buf);
	return className;
}

std::string	JavaEnv::getClassName (jobject object)
{
	return getClassName(env->GetObjectClass(object));
}

template<>
DocumentRef	JavaEnv::createDocument (jobject obj)
{
	if (!obj)
		return DocumentRef();

	jclass classObj = env->GetObjectClass(obj);
	if (!classObj)
		throw std::runtime_error ("value() Classe non trouvée");
	jstring jclassName = (jstring)callMethod<jobject> (classObj,"getName","()Ljava/lang/String;");
	const char* buf = env->GetStringUTFChars(jclassName,NULL);
	std::string className(buf);
	env->ReleaseStringUTFChars(jclassName,buf);

        std::replace(className.begin(),className.end(),'.','/');
        return factoryRef->createSerializable<Document>(*this,className,obj);
}

template<>
jobject		JavaEnv::createObject (const DocumentRef doc)
{
	if (!doc)
		return NULL;

	std::string className(doc->className());
	if (className == "String")
		return env->NewStringUTF(boost::static_pointer_cast<String>(doc)->get().c_str());
	if (className == "Integer")
	{
		jclass classObj = env->FindClass ("Ljava/lang/Integer;");
		if (!classObj)
			retinThrowException1 ("Classe %s non trouvée",className.c_str());
		jmethodID methodID = env->GetMethodID(classObj, "<init>", "(I)V");
		if (!methodID)
			retinThrowException1 ("Constructeur de %s non trouvé",className.c_str());
		jobject object = env->NewObject(classObj,methodID,boost::static_pointer_cast<Integer>(doc)->get());
		if (!object)
			retinThrowException1 ("Erreur lors de la création d'un %s",className.c_str());
		return object;
	}
	if (className == "Float")
	{
		jclass classObj = env->FindClass ("Ljava/lang/Float;");
		if (!classObj)
			retinThrowException1 ("Classe %s non trouvée",className.c_str());
		jmethodID methodID = env->GetMethodID(classObj, "<init>", "(F)V");
		if (!methodID)
			retinThrowException1 ("Constructeur de %s non trouvé",className.c_str());
		jobject object = env->NewObject(classObj,methodID,boost::static_pointer_cast<Float>(doc)->get());
		if (!object)
			retinThrowException1 ("Erreur lors de la création d'un %s",className.c_str());
		return object;
	}
	if (className == "Double")
	{
		jclass classObj = env->FindClass ("Ljava/lang/Double;");
		if (!classObj)
			retinThrowException1 ("Classe %s non trouvée",className.c_str());
		jmethodID methodID = env->GetMethodID(classObj, "<init>", "(D)V");
		if (!methodID)
			retinThrowException1 ("Constructeur de %s non trouvé",className.c_str());
		jobject object = env->NewObject(classObj,methodID,boost::static_pointer_cast<Double>(doc)->get());
		if (!object)
			retinThrowException1 ("Erreur lors de la création d'un %s",className.c_str());
		return object;
	}
	if (className == "ArrayList")
	{
		return createObject(boost::static_pointer_cast<ArrayList>(doc)->getValues());
	}
	if (className == "TreeMap")
	{
		return createObject(boost::static_pointer_cast<TreeMap>(doc)->getValues());
	}
		
	/*char jclassName[64];
	strcpy (jclassName,"Lretin/toolbox/document/");
	strcat (jclassName,doc->className());
	strcat (jclassName,";");
	jobject object = allocObject (jclassName);*/
        jobject object = allocObject (doc->fullJniClassName());
	doc->toJavaObject (*this,object);
	return object;
}

jobject	JavaEnv::allocObject (const char* jclassName)
{
	jclass classObj = env->FindClass (jclassName);
	if (!classObj)
		retinThrowException1 ("Classe %s non trouvée",jclassName);
	jobject object = env->AllocObject (classObj);
	if (!object)
		retinThrowException1 ("Erreur lors de la création de l'objet %s",jclassName);
	return object;
}


jobject	JavaEnv::createObject (const TreeMapRef doc)
{
	JavaMap map(env);
	jobject object = map.create("TreeMap");

	for (TreeMap::iterator i=doc->begin(),e=doc->end();i!=e;i++)
	{
		map.put ( i->first,i->second );
	}

	return object;
}


jobjectArray	JavaEnv::createObjectArray (const std::vector<std::string>& list)
{
	jobjectArray array = (jobjectArray)env->NewObjectArray(list.size(),env->FindClass("java/lang/String"),env->NewStringUTF(""));
	for (uint i=0;i<list.size();i++)
       		env->SetObjectArrayElement(array,i,env->NewStringUTF(list[i].c_str()));
 	return array;
}

jobjectArray	JavaEnv::createObjectArray (const std::vector<DocumentRef>& x)
{
	jobjectArray array = (jobjectArray)env->NewObjectArray(x.size(),env->FindClass("java/lang/Object"),NULL);
	for (uint i=0;i<x.size();i++)
       		env->SetObjectArrayElement(array,i,createObject(x[i]));
 	return array;
}


std::string	JavaEnv::createString (jobject object)
{
	if (!object)
		return "";
	const char* buf = env->GetStringUTFChars((jstring)object,NULL);
	std::string x = buf;
	env->ReleaseStringUTFChars((jstring)object,buf);
	return x;
}

vector<string>	JavaEnv::createStringVector (jobjectArray array)
{
	vector<string> list;
	jsize n = env->GetArrayLength(array);
	for (jsize i=0;i<n;i++) {
		jstring object = (jstring)env->GetObjectArrayElement(array,i);
		const char* buf = env->GetStringUTFChars(object,NULL);
		list.push_back(buf);
		env->ReleaseStringUTFChars(object,buf);
	}
	return list;
}

jfloat		JavaEnv::createFloat (jobject object)
{
	return callMethod<jfloat>(object,"floatValue","()F");
}

jdouble		JavaEnv::createDouble (jobject object)
{
	return callMethod<jdouble>(object,"doubleValue","()D");
}

template<>
std::string	JavaEnv::getField (jobject object,const char* fieldName)
{
	jclass classObj = env->GetObjectClass(object);
	if (!classObj)
		retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
	jfieldID fieldID = env->GetFieldID(classObj,fieldName,"Ljava/lang/String;");
	if (!fieldID)
		retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
	return createString( env->GetObjectField(object,fieldID) );
}

template<>
void		JavaEnv::setField (jobject object,const char* fieldName,const std::string& x)
{
	jclass classObj = env->GetObjectClass(object);
	if (!classObj)
		retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
	jfieldID fieldID = env->GetFieldID(classObj,fieldName,"Ljava/lang/String;");
	if (!fieldID)
		retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
	env->SetObjectField(object,fieldID,env->NewStringUTF (x.c_str()));
}


jfieldID	JavaEnv::getAbstractListFieldID (jobject object,const char* name)
{
	jclass classObj = env->GetObjectClass(object);
	if (!classObj)
		retinThrowException1 ("(fieldName=%s) Classe non trouvée",name);
	jfieldID fieldID = env->GetFieldID(classObj, name, "Ljava/util/AbstractList;");
	if (!fieldID)
	{
		env->ExceptionClear();
		fieldID = env->GetFieldID(classObj, name, "Ljava/util/List;");
	}
	if (!fieldID)
	{
		env->ExceptionClear();
		fieldID = env->GetFieldID(classObj, name, "Ljava/util/ArrayList;");
	}
	if (!fieldID)
	{
		env->ExceptionClear();
		fieldID = env->GetFieldID(classObj, name, "Ljava/util/Vector;");
	}
	if (!fieldID)
		retinThrowException1 ("(fieldName=%s) Champs invalide",name);
	return fieldID;
}


template<>
DocumentRef	JavaEnv::getDocumentField (jobject object,const char* fieldName)
{
	jclass classObj = env->GetObjectClass(object);
	if (!classObj)
		retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
	jfieldID fieldID = env->GetFieldID(classObj,fieldName,"Ljava/lang/Object;");
	if (!fieldID)
		retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
	return createDocument<Document>(env->GetObjectField(object,fieldID));
}
template<>
void		JavaEnv::setDocumentField (jobject object,const char* fieldName,const DocumentRef x)
{
	jclass classObj = env->GetObjectClass(object);
	if (!classObj)
		retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
	jfieldID fieldID = env->GetFieldID(classObj,fieldName,"Ljava/lang/Object;");
	if (!fieldID)
		retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
	env->SetObjectField(object,fieldID,createObject(x));
}

template<>
jobject		JavaEnv::callMethod (jobject obj,const char* methodName,const char* methodSign,...)
{
	jclass classObj = env->GetObjectClass(obj);
	if (!classObj)
		retinThrowException2 ("callMethod(jobject,%s,%s) La classe n'a pas été trouvée",methodName,methodSign);
	jmethodID methodID = env->GetMethodID(classObj, methodName, methodSign);
	if (!methodID)
		retinThrowException2 ("callMethod(jobject,%s,%s) La méthode n'a pas été trouvée",methodName,methodSign);
	va_list args;
	va_start(args,methodSign);
	jobject result = env->CallObjectMethodV(obj,methodID,args);
	va_end(args);
	return result;
}

// ****************************************************************

JavaBufferedImage::JavaBufferedImage (JNIEnv* env,jobject object) : env(env),je(env),object(object) 
{ 
	classObj = env->FindClass("Ljava/awt/image/BufferedImage;");
	if (!classObj)
		retinThrowException ("java.awt.image.BufferedImage");
	if (object)
		setObject (object);
}

void	JavaBufferedImage::setObject (jobject object_)
{
	object = object_;
}

int	JavaBufferedImage::getTypeCode (const char* typeName) const
{
	jfieldID typeFieldID = env->GetStaticFieldID(classObj,typeName,"I");
	if (!typeFieldID)
		retinThrowException ("java.awt.image.BufferedImage");
	return env->GetStaticIntField(classObj,typeFieldID);
}

int	JavaBufferedImage::getType () const
{
	jmethodID methodID = env->GetMethodID(classObj, "getType", "()I");
	if (!methodID)
		retinThrowException ("java.awt.image.BufferedImage");
	return env->CallIntMethod(object,methodID);
}

void	JavaBufferedImage::create (int width,int height,const char* typeName) 
{
	jmethodID methodID = env->GetMethodID(classObj, "<init>", "(III)V");
	if (!methodID)
		retinThrowException ("java.awt.image.BufferedImage");
	jint type = getTypeCode (typeName);
	object = env->NewObject(classObj,methodID,width,height,type);
	if (!object)
		retinThrowException ("java.awt.image.BufferedImage");
}

void	JavaBufferedImage::setData (const std::vector<uchar>& buf)
{
	jobject raster = je.callMethod<jobject>(object,"getRaster","()Ljava/awt/image/WritableRaster;");
	if (!raster)
		retinThrowException ("java.awt.image.BufferedImage");
	jobject dataBuffer = je.callMethod<jobject>(raster,"getDataBuffer","()Ljava/awt/image/DataBuffer;");
	if (!dataBuffer)
		retinThrowException ("java.awt.image.BufferedImage");
	jbyteArray data = (jbyteArray)je.callMethod<jobject>(dataBuffer,"getData","()[B");
	if (!data)
		retinThrowException ("java.awt.image.BufferedImage");
	env->SetByteArrayRegion (data,0,buf.size(),(const jbyte*)&buf[0]);
}

void	JavaBufferedImage::getData (std::vector<uchar>& buf)
{
	jobject raster = je.callMethod<jobject>(object,"getRaster","()Ljava/awt/image/WritableRaster;");
	if (!raster)
		retinThrowException ("java.awt.image.BufferedImage");
	jobject dataBuffer = je.callMethod<jobject>(raster,"getDataBuffer","()Ljava/awt/image/DataBuffer;");
	if (!dataBuffer)
		retinThrowException ("java.awt.image.BufferedImage");
	jbyteArray data = (jbyteArray)je.callMethod<jobject>(dataBuffer,"getData","()[B");
	if (!data)
		retinThrowException ("java.awt.image.BufferedImage");
	jsize n = env->GetArrayLength (data);
	buf.resize(n);
	env->GetByteArrayRegion (data,0,buf.size(),(jbyte*)&buf[0]);
}

}

using namespace retin;

JNIEXPORT jfloatArray JNICALL Java_retin_toolbox_document_serialization_Java_loadFloatBuffer
  (JNIEnv * env, jclass thisClass, jstring file, jint offset, jint size)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	ifstream f(je.createString(file).c_str(),ios::binary);
	f.seekg(offset,ios::beg);
	vector<float> buf(size);
	f.read ((char*)&buf[0],buf.size()*sizeof(float));
	f.close();
	return je.createArray(buf);

	RETIN_JAVA_METHOD_END
	return NULL;
}


JNIEXPORT void JNICALL Java_retin_toolbox_document_serialization_Java_saveFloatBuffer
  (JNIEnv * env, jclass thisClass, jstring file, jfloatArray buffer)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	vector<float> buf;
	je.createVector (buf,buffer);
	ofstream f(je.createString(file).c_str(),ios::binary);
	f.write ((const char*)&buf[0],buf.size()*sizeof(float));
	f.close();

	RETIN_JAVA_METHOD_END
}

JNIEXPORT jstring JNICALL Java_retin_toolbox_document_serialization_Java_shellExec
  (JNIEnv * env, jclass thisClass, jstring cmd)
{
	RETIN_JAVA_METHOD_BEGIN

	JavaEnv je(env);
	FILE* pipe = popen(je.createString(cmd).c_str(),"r");
	char buffer[128];
    	std::string result = "";
	while(!feof(pipe)) {
        	if(fgets(buffer, 128, pipe) != NULL)
        	        result += buffer;
	}
	pclose(pipe);
	return (jstring)je.createObject(result);

	RETIN_JAVA_METHOD_END
	return NULL;
}
