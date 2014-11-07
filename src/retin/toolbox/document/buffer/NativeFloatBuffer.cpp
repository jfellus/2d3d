/**
 * \file NativeFloatBuffer.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "NativeFloatBuffer.h"

#include "../serialization/Serializers.h"


RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(NativeFloatBuffer);

using namespace retin;

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_release
(JNIEnv * env, jobject thisObject) {
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    je.setArrayPtrField<float> (thisObject, "bufferPtr", (float*) NULL);

    RETIN_JAVA_METHOD_END
}


JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_resize
  (JNIEnv * env, jobject thisObject, jlong newsize, jboolean bKeepData)
{
    RETIN_JAVA_METHOD_BEGIN

    if (newsize < 0)
        retinThrowException1("Invalid size %l",newsize);
    
    JavaEnv je(env);
    float* oldbuffer = je.getArrayPtrField<float> (thisObject, "bufferPtr", false);
    jlong oldsize = je.getField<jlong> (thisObject, "length");
   
    float* newbuffer = NULL;
    if (newsize > 0) {
        newbuffer = new float[newsize];
        if (!newbuffer)
                retinThrowException1("Bad alloc size %l",newsize);
        if (bKeepData && oldbuffer)
            memcpy(newbuffer,oldbuffer,std::min(oldsize,newsize)*sizeof(float));
        else
            memset(newbuffer,0,newsize*sizeof(float));

    }
    if (oldbuffer)
        delete [] oldbuffer;
    
    je.setArrayPtrField<float> (thisObject,"bufferPtr",newbuffer);
    je.setField<jlong> (thisObject,"length",newsize);

    RETIN_JAVA_METHOD_END    
}

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_setValues
  (JNIEnv * env, jobject thisObject, jlong i0, jfloatArray array)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    float* buffer = je.getArrayPtrField<float> (thisObject, "bufferPtr");
    if (!buffer)
        retinThrowException("Buffer is null");
    
    jlong size = je.getField<jlong> (thisObject, "length");    
    jlong n = env->GetArrayLength (array);
    if ( (i0+n) > size )
        retinThrowException3("Buffer is too small (%d+%d > %d)",i0,n,size);
    
    je.fillVector<float>( buffer+i0,n,array );
    
    RETIN_JAVA_METHOD_END       
}

JNIEXPORT void JNICALL Java_retin_toolbox_document_buffer_NativeFloatBuffer_cpy
  (JNIEnv * env, jobject thisObject, jfloatArray jArray, jlong jSeek, jlong jSize){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    if (jSeek < 0)
        retinThrowException1("Invalid seek %l",jSeek);
    if (jSize < 0)
        retinThrowException1("Invalid size %l",jSeek);
    
    float* oldbuffer = je.getArrayPtrField<float> (thisObject, "bufferPtr", false);
    jlong oldsize = je.getField<jlong> (thisObject, "length");
    
    float* newbuffer = NULL;
    
    jlong n = env->GetArrayLength (jArray);
    if (jSeek+jSize > n)
        retinThrowException("seek + size is too large");
    
    if (oldbuffer && oldsize == jSize){
        newbuffer = oldbuffer;
    }
    else {
        newbuffer = new float[jSize];
        if (!newbuffer)
                retinThrowException1("Bad alloc size %l",jSize);
        if (oldbuffer)
                delete [] oldbuffer;
    }
    
    je.fillVectorWithArrayRegion<float>(newbuffer, jSeek, jSize, jArray);
    
    je.setArrayPtrField<float> (thisObject, "bufferPtr", newbuffer);
    je.setField<jlong> (thisObject, "length", jSize);
    
    RETIN_JAVA_METHOD_END   
}
