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
 * \file Java.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Java_h__
#define __retin_Java_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/TreeMap.h"
#include "retin/toolbox/document/serialization/SerializableObjectFactory.h"
#include "retin/toolbox/core/auto_array_ptr.h"

#include <string.h>

#define RETIN_JAVA_METHOD_BEGIN \
	try \
	{

#define RETIN_JAVA_METHOD_FINALIZE(code) \
	} \
	catch (std::exception& e) \
	{ \
		code \
		jclass clsException = env->FindClass("java/lang/Exception"); \
		env->ThrowNew (clsException,e.what()); \
		env->DeleteLocalRef (clsException); \
	} 

#define RETIN_JAVA_METHOD_END \
	} \
	catch (std::exception& e) \
	{ \
		jclass clsException = env->FindClass("java/lang/Exception"); \
		env->ThrowNew (clsException,e.what()); \
		env->DeleteLocalRef (clsException); \
	} 

#include "JavaTraits.h"

namespace retin
{

class	JavaArrayPtr
{
public:
virtual	void		release (JNIEnv*) = 0;
virtual ~JavaArrayPtr() {}
};

class	JavaArrayCriticalPtr : public JavaArrayPtr
{
	void*		ptr;
	jarray		array;
public:
	JavaArrayCriticalPtr (void* ptr,jarray array) : ptr(ptr),array(array)  { }
virtual	void		release (JNIEnv* env) { env->ReleasePrimitiveArrayCritical(array,ptr,0); }
};

template<class T>
class	JavaArrayElementPtr : public JavaArrayPtr
{
	T*		ptr;
	typename java_traits<T>::array_type 	array;
public:
	JavaArrayElementPtr (T* ptr,typename java_traits<T>::array_type array) : ptr(ptr),array(array)  { }
virtual	void		release (JNIEnv* env) { java_traits<T>::releaseArrayElements(env,array,ptr,0); }
};

class	JavaEnv
{
protected:
	JNIEnv* env;
	SerializableObjectFactoryRef	factoryRef;

	std::vector<jobject>		globalRefs;

	bool		bArrayCopy;
	bool		bAllowPrimitiveArrayCritical;
	std::vector<JavaArrayPtr*>	arrayPtrs;

public:
	JavaEnv (JNIEnv* env,bool bAllowPrimitiveArrayCritical=false);
	~JavaEnv ();

	JNIEnv*			getEnv() const { return env; }

	jobject		registerReference (jobject object);
	void		unregisterReferences ();

	void		setArrayCopy (bool b) { bArrayCopy = b; }
	void		setAllowPrimitiveArrayCritical (bool b) { bAllowPrimitiveArrayCritical = b; } 

	template<class T>
	T*		registerPrimitiveArrayCritical (typename java_traits<T>::array_type array)
	{
		T* p;
		jboolean isCopy;
		if (!bAllowPrimitiveArrayCritical)
		{
			p = java_traits<T>::getArrayElements (env,array,&isCopy);
			arrayPtrs.push_back( new JavaArrayElementPtr<T>(p,array) );
		}
		else
		{
			p = (T*)env->GetPrimitiveArrayCritical(array,&isCopy);
			arrayPtrs.push_back( new JavaArrayCriticalPtr(p,array) );
		}
		return p;
	}

	void		unregisterArrayPtrs();

	bool		isInstanceOf (jobject object,const char* className)
	{
		jclass classObj = env->FindClass(className);
		if (!classObj)
			retinThrowException1 ("Classe %s non trouvée",className);
		return env->IsInstanceOf (object,classObj);		
	}

	std::string		createString (jobject object);
	std::vector<std::string>createStringVector (jobjectArray array);
	jfloat			createFloat (jobject object);
	jdouble			createDouble (jobject object);

	template<class T>
	void			fillVector (T* x,int n,typename java_traits<T>::array_type array)
	{
		if (env->GetArrayLength (array) != n)
			retinThrowException("Invalid dimension");
//inline void GetArrayRegion(JNIEnv *env, ArrayType array, jsize start,. jsize len, NativeType *buf)
		java_traits<T>::getArrayRegion (env,array,0,n,x);
	}
        template<class T>
	void			fillVectorWithArrayRegion (T* x, jsize start, jsize len,typename java_traits<T>::array_type array)
	{
//inline void GetArrayRegion(JNIEnv *env, ArrayType array, jsize start, jsize len, NativeType *buf)
                java_traits<T>::getArrayRegion (env,array,start,len,x);
	}        
	template<class T>
	void			createVector (std::vector<T>& x,typename java_traits<T>::array_type array)
	{
		jsize n = env->GetArrayLength (array);
		x.resize(n);
		java_traits<T>::getArrayRegion (env,array,0,n,&x[0]);
	}
	template<class T>
	void			createVector (retin::auto_array_ptr<T>& x,typename java_traits<T>::array_type array)
	{
		if (bArrayCopy)
		{
			jsize n = env->GetArrayLength (array);
			x.resize(n);
			java_traits<T>::getArrayRegion (env,array,0,n,&x[0]);
		}
		else
			x.set(registerPrimitiveArrayCritical<T>(array),env->GetArrayLength (array));
	}

	template<class T>
	boost::shared_ptr<T> 	createDocument (jobject obj)
	{
		if (!obj)
			return boost::shared_ptr<T>();

		jclass classObj = env->GetObjectClass(obj);
		if (!classObj)
			throw std::runtime_error ("JavaEnv::createDocument<"+std::string(T::staticClassName())+">() Classe non trouvée");
		jstring jclassName = (jstring)callMethod<jobject> (classObj,"getName","()Ljava/lang/String;");
		const char* buf = env->GetStringUTFChars(jclassName,NULL);
		std::string className(buf);
		env->ReleaseStringUTFChars(jclassName,buf);

                std::replace(className.begin(),className.end(),'.','/');
                return factoryRef->createSerializable<T>(*this,className,obj);
	}
	template<class T>
	void		createDocument (std::vector<boost::shared_ptr<T> >& x,jobject vector)
	{
		x.clear ();

		// size = vector.size();
		jint size = callMethod<jint> (vector,"size","()I");

		// méthodes Iterator::hasNext () et Iterator::next ();
		jclass classObj = env->GetObjectClass(vector);
		if (!classObj)
			throw std::runtime_error ("La classe Vector n'a pas été trouvée");
		jmethodID getMethodID = env->GetMethodID(classObj, "get", "(I)Ljava/lang/Object;");
		if (!getMethodID)
			throw std::runtime_error ("La méthode Vector::get(int) n'a pas été trouvée");

		for (jint i=0;i<size;i++)
		{
			x.push_back ( createDocument<T>(env->CallObjectMethod(vector,getMethodID,i)) );
		}
	}
	template<class Key,class T>
	void		createDocument (std::map<Key,boost::shared_ptr<T> >& x,jobject map)
	{
		x.clear ();

		// set = map.entrySet();
		jobject set = callMethod<jobject> (map,"entrySet","()Ljava/util/Set;");
		// iterator = set.iterator();
		jobject iterator = callMethod<jobject> (set,"iterator","()Ljava/util/Iterator;");

		// méthodes Iterator::hasNext () et Iterator::next ();
		jclass classObj = env->GetObjectClass(iterator);
		if (!classObj)
			throw std::runtime_error ("La classe Iterator n'a pas été trouvée");
		jmethodID hasNextMethodID = env->GetMethodID(classObj, "hasNext", "()Z");
		if (!hasNextMethodID)
			throw std::runtime_error ("La méthode Iterator::hasNext() n'a pas été trouvée");
		jmethodID nextMethodID = env->GetMethodID(classObj, "next", "()Ljava/lang/Object;");
		if (!nextMethodID)
			throw std::runtime_error ("La méthode Iterator::next() n'a pas été trouvée");

		jmethodID getKeyMethodID = NULL,getValueMethodID = NULL;
		while (true)
		{
			if ( !env->CallBooleanMethod(iterator,hasNextMethodID) )
				break;
			jobject entry = env->CallObjectMethod(iterator,nextMethodID);

			if (!getKeyMethodID)
			{
				// méthodes Map.Entry::getKey () et getValue ();
				jclass classObj = env->GetObjectClass(entry);
				getKeyMethodID = env->GetMethodID(classObj, "getKey", "()Ljava/lang/Object;");
				if (!getKeyMethodID)
					throw std::runtime_error ("La méthode Map.Entry::getKey() n'a pas été trouvée");
				getValueMethodID = env->GetMethodID(classObj, "getValue", "()Ljava/lang/Object;");
				if (!getValueMethodID)
					throw std::runtime_error ("La méthode Map.Entry::getValue() n'a pas été trouvée");
			}

			// Récupère la clef
			std::string id;
			jstring key = (jstring)env->CallObjectMethod(entry,getKeyMethodID);
			if (!key)
				retinThrowException1 ("(map< Key,shared_ptr<%s> >) L'un des éléments du map n'a pas de clef",T::staticClassName());
			const char* buf = env->GetStringUTFChars(key,NULL);
			id = buf;
			env->ReleaseStringUTFChars(key,buf);

			// Ajoute au map
			x.insert( make_pair(id, createDocument<T> (env->CallObjectMethod(entry,getValueMethodID)) ) );
		}
	}

	template<class T>
	typename java_traits<T>::array_type	createArray (const T* x,int n)
	{
		typename java_traits<T>::array_type array = java_traits<T>::newArray (env,n);
		if (!array)
			retinThrowException("Allocation error");
		java_traits<T>::setArrayRegion (env,array,0,n,x);
		return array;
	}
	template<class T>
	typename java_traits<T>::array_type	createArray (const std::vector<T>& x)
	{
		if (x.size() >= 0x7FFFFFFF)
			retinThrowException("Buffer too large");
		return createArray(&x[0],x.size());
	}
	template<class T>
	typename java_traits<T>::array_type	createArray (const retin::auto_array_ptr<T>& x)
	{
		if (x.size() >= 0x7FFFFFFF)
			retinThrowException("Buffer too large");
                return createArray(x.get(),x.size());
		/*typename java_traits<T>::array_type array = java_traits<T>::newArray (env,x.size());
		if (x)
			java_traits<T>::setArrayRegion (env,array,0,x.size(),x.get());
		else if (bArrayCopy) {
			retinThrowException ("C array is empty");
		}
		else
			x.set( registerPrimitiveArrayCritical<T>(array) );
		return array;*/
	}

	jobject		allocObject (const char* className);

	template<class T>
	jobject		createObject (const T& x)
	{
		retinThrowException ("Non implanté");
	}
	template<class T>
	jobject		createObject (const boost::shared_ptr<T> doc)
	{
		if (!doc)
			return NULL;
                jclass classObj = env->FindClass (doc->fullJniClassName());
		if (!classObj)
			retinThrowException1 ("(T=%s) Classe non trouvée",T::staticClassName());
		jobject object = env->AllocObject (classObj);
		if (!object)
			retinThrowException1 ("(T=%s) Erreur lors de la création de l'objet",T::staticClassName());
		doc->toJavaObject (*this,object);
		return object;
	}
	jobject		createObject (const TreeMapRef doc);
	template<class T>
	jobject		createObject (const std::vector<boost::shared_ptr<T> >& x)
	{
		jclass classObj = env->FindClass("Ljava/util/ArrayList;");
		if (!classObj)
			retinThrowException1 ("(T=%s) Classe java.util.ArrayList non trouvée",T::staticClassName());
		jmethodID methodID = env->GetMethodID(classObj, "<init>", "()V");
		if (!methodID)
			retinThrowException1 ("(T=%s) Constructeur de java.util.ArrayList non trouvé",T::staticClassName());
		jobject object = env->NewObject(classObj,methodID);
		if (!object)
			retinThrowException1 ("(T=%s) Erreur lors de la création",T::staticClassName());

		methodID = env->GetMethodID(classObj, "add", "(Ljava/lang/Object;)Z");
		if (!methodID)
			retinThrowException1 ("(T=%s) Méthode java.util.ArrayList::add non trouvée",T::staticClassName());
		for (uint i=0;i<x.size();i++)
			env->CallBooleanMethod(object,methodID,createObject(x[i]));

		return object;
	}
	template<class Key,class T>
	jobject		createObject (const std::map<Key,boost::shared_ptr<T> >& x)
	{
		jclass classObj = env->FindClass("Ljava/util/TreeMap;");
		if (!classObj)
			retinThrowException1 ("(T=%s) Classe java.util.TreeMap non trouvée",T::staticClassName());
		jmethodID methodID = env->GetMethodID(classObj, "<init>", "()V");
		if (!methodID)
			retinThrowException1 ("(T=%s) Constructeur de java.util.TreeMap non trouvé",T::staticClassName());
		jobject object = env->NewObject(classObj,methodID);
		if (!object)
			retinThrowException1 ("(T=%s) Erreur lors de la création",T::staticClassName());

		methodID = env->GetMethodID(classObj, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
		if (!methodID)
			retinThrowException1 ("(T=%s) Méthode java.util.TreeMap::put non trouvée",T::staticClassName());

		for (typename std::map<Key,boost::shared_ptr<T> >::const_iterator i=x.begin(),e=x.end();i!=e;i++)
			env->CallObjectMethod(object,methodID,createObject(i->first),createObject(i->second));

		return object;
	}
	jobjectArray	createObjectArray (const std::vector<std::string>& list);
	jobjectArray	createObjectArray (const std::vector<DocumentRef>& x);


	template<class T>
	T		getField (jobject object,const char* fieldName)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
		jfieldID fieldID = env->GetFieldID(classObj,fieldName,java_traits<T>::sign());
		if (!fieldID)
			retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
		return java_traits<T>::getField(env,object,fieldID);
	}
	template<class T>
	void		setField (jobject object,const char* fieldName,const T& x)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
		jfieldID fieldID = env->GetFieldID(classObj,fieldName,java_traits<T>::sign());
		if (!fieldID)
			retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
		java_traits<T>::setField (env,object,fieldID,x);
	}

	jobject		getObjectField (jobject object,const char* fieldName,const char* classSign)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException2 ("(fieldName=%d,classSign=%s) Classe non trouvée",fieldName,classSign);
		jfieldID fieldID = env->GetFieldID(classObj,fieldName,classSign);
		if (!fieldID)
			retinThrowException2 ("(fieldName=%d,classSign=%s) Champs invalide",fieldName,classSign);
		return env->GetObjectField(object,fieldID);
	}


	template<class T>
	typename java_traits<T>::array_type	getArrayField (jobject object,const char* fieldName)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
		jfieldID fieldID = env->GetFieldID(classObj, fieldName, java_traits<T>::arraySign());
		if (!fieldID)
			retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
		typename java_traits<T>::array_type array = (typename java_traits<T>::array_type)env->GetObjectField(object,fieldID);
		if (!array) 
			retinThrowException1 ("(fieldName=%s) Pas de tableau",fieldName);
		return array;
	}
	template<class T>
	void		getArrayField (T* x,int n,jobject object,const char* fieldName)
	{
		fillVector (x,n,getArrayField<T>(object,fieldName));
	}
	template<class T>
	void		getArrayField (std::vector<T>& x,jobject object,const char* fieldName)
	{
		createVector (x,getArrayField<T>(object,fieldName));
	}
	template<class T>
	void		getArrayField (retin::auto_array_ptr<T>& x,jobject object,const char* fieldName)
	{
		createVector (x,getArrayField<T>(object,fieldName));
	}

	template<class T>
	void		setArrayField (jobject object,const char* fieldName,typename java_traits<T>::array_type array)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException1 ("(fieldName=%s) Classe non trouvée",fieldName);
		jfieldID fieldID = env->GetFieldID(classObj, fieldName, java_traits<T>::arraySign());
		if (!fieldID)
			retinThrowException1 ("(fieldName=%s) Champs invalide",fieldName);
		env->SetObjectField(object,fieldID,array);
	}
	template<class T>
	void		setArrayField (jobject object,const char* fieldName,const T* x,int n)
	{
		setArrayField<T> (object,fieldName,createArray(x,n));
	}
	template<class T>
	void		setArrayField (jobject object,const char* fieldName,const std::vector<T>& x)
	{
		setArrayField<T> (object,fieldName,createArray(x));
	}
	template<class T>
	void		setArrayField (jobject object,const char* fieldName,const retin::auto_array_ptr<T>& x)
	{
		setArrayField<T> (object,fieldName,createArray(x));
	}

	template<class T>
	boost::shared_ptr<T>	getDocumentField (jobject object,const char* fieldName)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException2 ("(T=%s,fieldName=%s) Classe non trouvée",T::staticClassName(),fieldName);
		/*char tmp[64];
		strcpy (tmp,"Lretin/toolbox/document/");
		strcat (tmp,T::staticClassName());
		strcat (tmp,";");*/
		jfieldID fieldID = env->GetFieldID(classObj,fieldName,T::staticFullJniClassName());
		if (!fieldID)
			retinThrowException2 ("(T=%s,fieldName=%s) Champs invalide",T::staticClassName(),fieldName);
		return createDocument<T>(env->GetObjectField(object,fieldID));
	}
	template<class T>
	void		setDocumentField (jobject object,const char* fieldName,const boost::shared_ptr<T> x)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException2 ("(T=%s,fieldName=%s) Classe non trouvée",T::staticClassName(),fieldName);
		/*char tmp[64];
		strcpy (tmp,"Lretin/toolbox/document/");
		strcat (tmp,T::staticClassName());
		strcat (tmp,";");*/
		jfieldID fieldID = env->GetFieldID(classObj,fieldName,T::staticFullJniClassName());
		if (!fieldID)
			retinThrowException2 ("(T=%s,fieldName=%s) Champs invalide",T::staticClassName(),fieldName);
		env->SetObjectField(object,fieldID,createObject<T>(x));
	}

	jfieldID	getAbstractListFieldID (jobject object,const char* name);
	template<class T>
	void		getDocumentField (std::vector<boost::shared_ptr<T> >& x,jobject object,const char* fieldName)
	{
		jfieldID fieldID = getAbstractListFieldID (object,fieldName);
		jobject vector = (jobject)env->GetObjectField(object,fieldID);
		createDocument (x,vector);
	}
	template<class T>
	void		setDocumentField (jobject object,const char* fieldName,const std::vector<boost::shared_ptr<T> >& x)
	{
		jfieldID fieldID = getAbstractListFieldID (object,fieldName);
		env->SetObjectField (object,fieldID,createObject(x));
	}
	template<class T>
	void		getListField (std::vector<T>& x,jobject object,const char* fieldName)
	{
		retinThrowException ("Non implanté");
		/*jfieldID fieldID = getAbstractListFieldID (object,fieldName);
		jobject vector = (jobject)env->GetObjectField(object,fieldID);
		createDocument (x,vector);*/
	}
	template<class T>
	void		setListField (jobject object,const char* fieldName,const std::vector<T>& x)
	{
		retinThrowException ("Non implanté");
		/*jfieldID fieldID = getAbstractListFieldID (object,fieldName);
		env->SetObjectField (object,fieldID,createObject(x));*/
	}

	template<class Key,class T>
	void		getDocumentField (std::map<Key,boost::shared_ptr<T> >& x,jobject object,const char* name)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException2 ("(T=%s,fieldName=%s) Classe non trouvée",T::staticClassName(),name);
		jfieldID fieldID = env->GetFieldID(classObj, name, "Ljava/util/TreeMap;");
		if (!fieldID)
			retinThrowException2 ("(T=%s,fieldName=%s) Champs invalide",T::staticClassName(),name);
		jobject map = (jobject)env->GetObjectField(object,fieldID);
		if (!map) 
			retinThrowException2 ("(T=%s,fieldName=%s) Empty object",T::staticClassName(),name);

		createDocument (x,map);
	}
	template<class Key,class T>
	void		setDocumentField (jobject object,const char* fieldName,const std::map<Key,boost::shared_ptr<T> >& x)
	{
		jclass classObj = env->GetObjectClass(object);
		if (!classObj)
			retinThrowException2 ("(T=%s,fieldName=%s) Classe non trouvée",T::staticClassName(),fieldName);
		jfieldID fieldID = env->GetFieldID(classObj,fieldName,"Ljava/util/TreeMap;");
		if (!fieldID)
			retinThrowException2 ("(T=%s,fieldName=%s) Champs invalide",T::staticClassName(),fieldName);
		env->SetObjectField (object,fieldID,createObject(x));
	}

	template<class T>
	T		callMethod (jobject obj,const char* methodName,const char* methodSign,...)
	{
		jclass classObj = env->GetObjectClass(obj);
		if (!classObj)
			retinThrowException2 ("callMethod(jobject,%s,%s) La classe n'a pas été trouvée",methodName,methodSign);
		jmethodID methodID = env->GetMethodID(classObj, methodName, methodSign);
		if (!methodID)
			retinThrowException2 ("callMethod(jobject,%s,%s) La méthode n'a pas été trouvée",methodName,methodSign);
		va_list args;
		va_start(args,methodSign);
		T result = java_traits<T>::callMethod(env,obj,methodID,args);
		va_end(args);
		return result;
	}

	template<class T>
	T*	getPtrField (jobject object,const char* fieldName)
	{
		jlong x = getField<jlong> (object,fieldName);
		if (x == 0)
			throw std::runtime_error ("getPtrField("+std::string(fieldName)+") Pointeur nul");
		return (T*)x;
	}
	template<class T>
	void	setPtrField (jobject object,const char* fieldName,T* ptr)
	{
		jlong x = getField<jlong> (object,fieldName);
		if (x != 0)
		{
			delete (T*)x;
		}
		setField (object,fieldName,(jlong)ptr);
	}

	template<class T>
	T*	getArrayPtrField (jobject object,const char* fieldName, bool checkNotNull=true)
	{
		jlong x = getField<jlong> (object,fieldName);
		if (checkNotNull && x == 0)
			throw std::runtime_error ("getArrayPtrField("+std::string(fieldName)+") Pointeur nul");
		return (T*)x;
	}
	template<class T>
	void	setArrayPtrField (jobject object,const char* fieldName,T* ptr)
	{
		jlong x = getField<jlong> (object,fieldName);
		if (x != 0)
		{
			delete [] (T*)x;
		}
		setField (object,fieldName,(jlong)ptr);
	}
        
	std::string	getClassName (jclass classObj);
	std::string	getClassName (jobject object);

};

template<>
DocumentRef	JavaEnv::createDocument (jobject object);
template<>
inline jobject	JavaEnv::createObject (const std::string& str) { return env->NewStringUTF(str.c_str()); }
template<>
jobject		JavaEnv::createObject (const DocumentRef doc);

template<>
DocumentRef	JavaEnv::getDocumentField (jobject object,const char* fieldName);
template<>
void		JavaEnv::setDocumentField (jobject object,const char* fieldName,const DocumentRef x);

template<>
std::string	JavaEnv::getField (jobject object,const char* fieldName);
template<>
void		JavaEnv::setField (jobject object,const char* fieldName,const std::string& x);

template<>
jobject		JavaEnv::callMethod (jobject obj,const char* methodName,const char* methodSign,...);

class	JavaMap
{
	JNIEnv*		env;
	JavaEnv 	je;
	jobject		object;
	jmethodID 	putMethodID;
public:
	JavaMap (JNIEnv* env) : env(env),je(env),object(0) { }

	jobject		create (const std::string& mapClassName)
	{
		jclass classObj = env->FindClass(("Ljava/util/"+mapClassName+";").c_str());
		if (!classObj)
			retinThrowException1 ("Classe java.util.%s non trouvée",mapClassName.c_str());
		jmethodID methodID = env->GetMethodID(classObj, "<init>", "()V");
		if (!methodID)
			retinThrowException1 ("Constructeur de java.util.%s non trouvé",mapClassName.c_str());
		object = env->NewObject(classObj,methodID);
		if (!object)
			retinThrowException1 ("Erreur lors de la création d'un java.util.%s",mapClassName.c_str());
		putMethodID = env->GetMethodID(classObj, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
		if (!putMethodID)
			retinThrowException1 ("Méthode java.util.%s::put non trouvée",mapClassName.c_str());
		return object;
	}

	template<class Key,class T>
	void	put (const Key& key,const boost::shared_ptr<T> x)
	{
		env->CallObjectMethod(object,putMethodID,je.createObject(key),je.createObject(x));
	}

};

class	JavaBufferedImage {
	JNIEnv*		env;
	JavaEnv 	je;
	jclass		classObj;
	jobject		object;
public:
	JavaBufferedImage (JNIEnv* env,jobject object=NULL);

	int		getTypeCode (const char* typeName) const;
	int		getType () const;
	bool		isType (const char* typeName) const { return getType() == getTypeCode(typeName); }
	
	void		setObject (jobject object);
	jobject		getObject () const { return object; }
	
	void		create (int width,int height,const char* typeName);
	void		setData (const std::vector<uchar>& buf);
	void		getData (std::vector<uchar>& buf);
	
};


}

extern "C" {

JNIEXPORT jfloatArray JNICALL Java_retin_toolbox_document_serialization_Java_loadFloatBuffer
  (JNIEnv *, jclass, jstring, jint, jint);

JNIEXPORT void JNICALL Java_retin_toolbox_document_serialization_Java_saveFloatBuffer
  (JNIEnv *, jclass, jstring, jfloatArray);

JNIEXPORT jstring JNICALL Java_retin_toolbox_document_serialization_Java_shellExec
  (JNIEnv *, jclass, jstring);

}


#endif
