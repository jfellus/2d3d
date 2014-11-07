/**
 * \file JNIOutputSerializer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_JNIOutputSerializer_h__
#define __retin_JNIOutputSerializer_h__

#include "retin/toolbox/document/serialization/Java.h"
#include <map>

namespace retin
{

class	JNIOutputSerializer
{
	JavaEnv&	je;
	jobject		obj;
	jclass		objClass;

public:
	JNIOutputSerializer (JavaEnv& je,jobject obj);

	template<class T>
	void		object (const char* name,const T& x)
	{
		je.setField (obj,name,x);
	}

	template<class T>
	void		object (const char* name,const boost::shared_ptr<T>& x)
	{
		je.setDocumentField (obj,name,x);
	}

	template<class T>
	void		object (const char* name,const std::vector<T>& x)
	{
		retinThrowException("Non implanté");
	}

	template<class T>
	void		object (const char* name,const std::vector<boost::shared_ptr<T> >& x)
	{
		je.setDocumentField (obj,name,x);
	}
	
	template<class Key,class T>
	void		object (const char* name,const std::map<Key,boost::shared_ptr<T> >& x)
	{
		je.setDocumentField (obj,name,x);
	}
        
        template<class Key,class T>
	void		object (const char* name,const std::map<Key,T>& x)
	{
		retinThrowException("Non implanté");
	}

	template<class T>
	void		vector (const char* name,const std::vector<T>& x,int dim)
	{
		je.setArrayField(obj,name,x);
	}

	template<class T>
	void		vector (const char* name,const retin::auto_array_ptr<T>& x,int dim)
	{
		je.setArrayField(obj,name,x);
	}

        template<class T>
	void		matrix (const char* name,const retin::auto_array_ptr<T>& x,int dim,int n)
	{
		je.setArrayField (obj,name,x);
	}

	template<class T>
	void		matrix (const char* name,const std::vector<T>& x,int dim,int n)
	{
		je.setArrayField (obj,name,x);
	}
};

}

#endif

