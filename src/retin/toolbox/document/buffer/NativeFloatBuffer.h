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
