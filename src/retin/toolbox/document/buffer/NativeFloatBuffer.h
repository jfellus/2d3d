/**
 * \file NativeFloatBuffer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_NativeFloatBuffer_h__
#define __retin_NativeFloatBuffer_h__

#include "../Document.h"

namespace retin
{

class	NativeFloatBuffer : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/buffer,NativeFloatBuffer);

        jlong           length;
	jlong		bufferPtr;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, NativeFloatBuffer& x)
	{
		s.object ("length",x.length);
		s.object ("bufferPtr",x.bufferPtr);
	}

public:
	NativeFloatBuffer () : bufferPtr(0) { }
        NativeFloatBuffer (jlong size) : length(size) { bufferPtr = (long)new float[size]; }
        NativeFloatBuffer (float* ptr,jlong size) : length(size) { bufferPtr = (long)ptr; }

        jlong           size() const { return length; }
	float*          get() const { return (float*)bufferPtr; }
        void            release() { bufferPtr = 0; length = 0;}
};

typedef boost::shared_ptr<NativeFloatBuffer> NativeFloatBufferRef;

}

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_release
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_resize
  (JNIEnv *, jobject, jlong, jboolean);

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_setValues
  (JNIEnv *, jobject, jlong, jfloatArray);

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_cpy
  (JNIEnv *, jobject, jfloatArray, jlong, jlong);

}


#endif


