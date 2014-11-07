/**
 * \file JavaTraits.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_JavaTraits_h__
#define __retin_JavaTraits_h__

#include <jni.h>

namespace retin {

template<class T>
class	java_traits { };

template<> 
class	java_traits<jbyte>
{
public:
	typedef		jbyte		this_type;
	typedef		jbyteArray	array_type;
static	const char* 	sign() { return "B"; }
static	const char*	arraySign() { return "[B"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetByteField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetByteField(object,fieldID); }
static	array_type	newArray (JNIEnv* env,jsize len) { return env->NewByteArray(len); }
static	void		setArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,const this_type* x) { env->SetByteArrayRegion(array,start,len,x); }
static	void		getArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,this_type* x) { env->GetByteArrayRegion(array,start,len,x); }
static	this_type*	getArrayElements (JNIEnv* env,array_type array,jboolean* isCopy=NULL) { return env->GetByteArrayElements(array,isCopy); }
static	void		releaseArrayElements (JNIEnv* env,array_type array,this_type* x,jbyte mode=0) { env->ReleaseByteArrayElements(array,x,mode); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallByteMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};

template<> 
class	java_traits<jchar>
{
public:
	typedef		jchar		this_type;
	typedef		jcharArray	array_type;
static	const char* 	sign() { return "C"; }
static	const char*	arraySign() { return "[C"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetCharField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetCharField(object,fieldID); }
static	array_type	newArray (JNIEnv* env,jsize len) { return env->NewCharArray(len); }
static	void		setArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,const this_type* x) { env->SetCharArrayRegion(array,start,len,x); }
static	void		getArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,this_type* x) { env->GetCharArrayRegion(array,start,len,x); }
static	this_type*	getArrayElements (JNIEnv* env,array_type array,jboolean* isCopy=NULL) { return env->GetCharArrayElements(array,isCopy); }
static	void		releaseArrayElements (JNIEnv* env,array_type array,this_type* x,jchar mode=0) { env->ReleaseCharArrayElements(array,x,mode); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallCharMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};

template<> 
class	java_traits<jboolean>
{
public:
	typedef		jboolean		this_type;
	typedef		jbooleanArray           array_type;
static	const char* 	sign() { return "Z"; }
static	const char*	arraySign() { return "[Z"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetBooleanField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetBooleanField(object,fieldID); }
static	array_type	newArray (JNIEnv* env,jsize len) { return env->NewBooleanArray(len); }
static	void		setArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,const this_type* x) { env->SetBooleanArrayRegion(array,start,len,x); }
static	void		getArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,this_type* x) { env->GetBooleanArrayRegion(array,start,len,x); }
static	this_type*	getArrayElements (JNIEnv* env,array_type array,jboolean* isCopy=NULL) { return env->GetBooleanArrayElements(array,isCopy); }
static	void		releaseArrayElements (JNIEnv* env,array_type array,this_type* x,jchar mode=0) { env->ReleaseBooleanArrayElements(array,x,mode); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallBooleanMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};

template<> 
class	java_traits<jint>
{
public:
	typedef		jint		this_type;
	typedef		jintArray	array_type;
static	const char* 	sign() { return "I"; }
static	const char*	arraySign() { return "[I"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetIntField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetIntField(object,fieldID); }
static	array_type	newArray (JNIEnv* env,jsize len) { return env->NewIntArray(len); }
static	void		setArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,const this_type* x) { env->SetIntArrayRegion(array,start,len,x); }
static	void		getArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,this_type* x) { env->GetIntArrayRegion(array,start,len,x); }
static	this_type*	getArrayElements (JNIEnv* env,array_type array,jboolean* isCopy=NULL) { return env->GetIntArrayElements(array,isCopy); }
static	void		releaseArrayElements (JNIEnv* env,array_type array,this_type* x,jint mode=0) { env->ReleaseIntArrayElements(array,x,mode); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallIntMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};

template<> 
class	java_traits<jlong>
{
public:
	typedef		jlong		this_type;
	typedef		jlongArray	array_type;
static	const char* 	sign() { return "J"; }
static	const char*	arraySign() { return "[J"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetLongField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetLongField(object,fieldID); }
static	array_type	newArray (JNIEnv* env,jsize len) { return env->NewLongArray(len); }
static	void		setArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,const this_type* x) { env->SetLongArrayRegion(array,start,len,x); }
static	void		getArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,this_type* x) { env->GetLongArrayRegion(array,start,len,x); }
static	this_type*	getArrayElements (JNIEnv* env,array_type array,jboolean* isCopy=NULL) { return env->GetLongArrayElements(array,isCopy); }
static	void		releaseArrayElements (JNIEnv* env,array_type array,this_type* x,jint mode=0) { env->ReleaseLongArrayElements(array,x,mode); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallLongMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};

template<> 
class	java_traits<jfloat>
{
public:
	typedef		jfloat		this_type;
	typedef		jfloatArray	array_type;
static	const char* 	sign() { return "F"; }
static	const char*	arraySign() { return "[F"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetFloatField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetFloatField(object,fieldID); }
static	array_type	newArray (JNIEnv* env,jsize len) { return env->NewFloatArray(len); }
static	void		setArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,const this_type* x) { env->SetFloatArrayRegion(array,start,len,x); }
static	void		getArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,this_type* x) { env->GetFloatArrayRegion(array,start,len,x); }
static	this_type*	getArrayElements (JNIEnv* env,array_type array,jboolean* isCopy=NULL) { return env->GetFloatArrayElements(array,isCopy); }
static	void		releaseArrayElements (JNIEnv* env,array_type array,this_type* x,jint mode=0) { env->ReleaseFloatArrayElements(array,x,mode); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallFloatMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};

template<> 
class	java_traits<jdouble>
{
public:
	typedef		jdouble		this_type;
	typedef		jdoubleArray	array_type;
static	const char* 	sign() { return "D"; }
static	const char*	arraySign() { return "[D"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetDoubleField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetDoubleField(object,fieldID); }
static	array_type	newArray (JNIEnv* env,jsize len) { return env->NewDoubleArray(len); }
static	void		setArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,const this_type* x) { env->SetDoubleArrayRegion(array,start,len,x); }
static	void		getArrayRegion (JNIEnv* env,array_type array,jsize start,jsize len,this_type* x) { env->GetDoubleArrayRegion(array,start,len,x); }
static	this_type*	getArrayElements (JNIEnv* env,array_type array,jboolean* isCopy=NULL) { return env->GetDoubleArrayElements(array,isCopy); }
static	void		releaseArrayElements (JNIEnv* env,array_type array,this_type* x,jint mode=0) { env->ReleaseDoubleArrayElements(array,x,mode); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallDoubleMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};

template<> 
class	java_traits<jobject>
{
public:
	typedef		jobject		this_type;
	typedef		jobjectArray	array_type;
static	const char* 	sign() { return "Ljava/lang/Object;"; }
static	const char*	arraySign() { return "[Ljava/lang/Object;"; }
static	void		setField (JNIEnv* env,jobject object,jfieldID fieldID,this_type x) { env->SetObjectField(object,fieldID,x); }
static	this_type	getField (JNIEnv* env,jobject object,jfieldID fieldID) { return env->GetObjectField(object,fieldID); }
static	this_type	callMethod (JNIEnv *env,jobject obj,jmethodID methodID,...) 
	{ 
		va_list args;
		va_start(args,methodID);
		this_type result = env->CallObjectMethodV(obj,methodID,args);
		va_end(args);
		return result;
	}	 
};


}

#endif
