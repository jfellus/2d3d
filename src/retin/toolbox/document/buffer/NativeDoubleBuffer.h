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
