/**
 * \file NativePointer.h
 * \author Romain Negrel
 * \version 4.0
 */

#ifndef __retin_NativePointer_h__
#define __retin_NativePointer_h__

#include "../Document.h"

namespace retin
{

class	NativePointer : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/basic,NativePointer);

	jlong 		ptr;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, NativePointer& x)
	{
		s.object ("ptr",x.ptr);
	}

public:
	NativePointer () : ptr((jlong)NULL) { }
        NativePointer (void * x) : ptr((jlong)x) { }

	void *		get() const { return (void *)ptr; }
	void		set(void * x) { ptr = (jlong)x; }
                
        template<class T>
	T *             getPointerAs ()
	{
		return (T*) ptr;
	}
};

typedef boost::shared_ptr<NativePointer> NativePointerRef;

}
#endif //__retin_NativePointer_h__