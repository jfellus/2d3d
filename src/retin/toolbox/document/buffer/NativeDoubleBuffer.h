/**
 * \file NativeDoubleBuffer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_NativeDoubleBuffer_h__
#define __retin_NativeDoubleBuffer_h__

#include "../Document.h"

namespace retin
{

class	NativeDoubleBuffer : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/buffer,NativeDoubleBuffer);

        jlong           length;
	jlong		bufferPtr;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, NativeDoubleBuffer& x)
	{
		s.object ("length",x.length);
		s.object ("bufferPtr",x.bufferPtr);
	}

public:
	NativeDoubleBuffer () : bufferPtr(0) { }
        NativeDoubleBuffer (jlong size) : length(size) { bufferPtr = (long)new double[size]; }
        NativeDoubleBuffer (double* ptr,jlong size) : length(size) { bufferPtr = (long)ptr; }

        jlong           size() const { return length; }
	double*          get() const { return (double*)bufferPtr; }
};

typedef boost::shared_ptr<NativeDoubleBuffer> NativeDoubleBufferRef;

}

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeDoubleBuffer_release
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeDoubleBuffer_resize
  (JNIEnv *, jobject, jlong, jboolean);

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeDoubleBuffer_setValues
  (JNIEnv *, jobject, jlong, jdoubleArray);

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeDoubleBuffer_cpy
  (JNIEnv *, jobject, jdoubleArray, jlong, jlong);

}


#endif


