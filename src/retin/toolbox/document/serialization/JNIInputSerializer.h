/**
 * \file JNIInputSerializer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_JNIInputSerializer_h__
#define __retin_JNIInputSerializer_h__

#include "retin/toolbox/document/serialization/Java.h"

#include <map>
#include <string.h>

namespace retin
{

class	JNIInputSerializer
{
	JavaEnv&	je;
	jobject 	obj;
	jclass		objClass;

public:
	JNIInputSerializer (JavaEnv& je,jobject obj);

	template<class T>
	void		object (const char* name,T& x)
	{
		x = je.getField<T>(obj,name);
	}

	template<class T>
	void		object (const char* name,boost::shared_ptr<T>& x)
	{
		x = je.getDocumentField<T> (obj,name);
	}

	template<class T>
	void		object (const char* name,std::vector<T>& x)
	{
		retinThrowException("Non implanté");
	}

	template<class T>
	void		object (const char* name,std::vector<boost::shared_ptr<T> >& x)
	{
		je.getDocumentField (x,obj,name);
	}

	template<class Key,class T>
	void		object (const char* name,std::map<Key,boost::shared_ptr<T> >& x)
	{
		je.getDocumentField (x,obj,name);
	}
        
        template<class Key,class T>
	void		object (const char* name,std::map<Key,T>& x)
	{
		retinThrowException("Non implanté");
	}

	template<class T>
	void		vector (const char* name,std::vector<T>& x,int dim)
	{
		je.getArrayField(x,(jobject)obj,name);
	}

        template<class T>
	void		vector (const char* name,retin::auto_array_ptr<T>& x,int dim)
	{
		je.getArrayField(x,obj,name);
	}

	template<class T>
	void		matrix (const char* name,std::vector<T>& x,int dim,int n)
	{
		je.getArrayField (x,obj,name);
	}

	template<class T>
	void		matrix (const char* name,retin::auto_array_ptr<T>& x,int dim,int n)
	{
		je.getArrayField (x,obj,name);
	}
};

}

#endif

