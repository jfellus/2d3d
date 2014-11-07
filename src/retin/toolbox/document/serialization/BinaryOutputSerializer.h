/**
 * \file BinaryOutputSerializer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_BinaryOutputSerializer_h__
#define __retin_BinaryOutputSerializer_h__

#include "retin/toolbox/document/serialization/Serializable.h"
#include "retin/toolbox/core/auto_array_ptr.h"

#include <vector>
#include <map>

namespace retin
{

class	BinaryOutputSerializer
{
	std::ostream&	s;

	void	writeBuffer (const char* buf,const size_t n);

	inline void	writeString (const char* pc) 
	{
		uint n = strlen(pc);
		if (n >= 256)
			retinThrowException1 ("String %s too large",pc);
		s.write ((const char*)&n,1);
		s.write (pc,n);
	}

public:
	BinaryOutputSerializer (std::ostream& s) : s(s) { }

	std::ostream&	stream() { return s; }
	void	header ();
	void	footer ();

	template<class T> 
	void		object (const char* name,const T& x);
	template<class T> 
	void		object (const char* name,const boost::shared_ptr<T> x);
	template<class T> 
	void		object (const char* name,const std::vector<T>& x);
	template<class T> 
	void		object (const char* name,const std::vector<boost::shared_ptr<T> >& x);
	template<class Key,class T> 
	void 		object (const char* name,const std::map<Key,boost::shared_ptr<T> >& x);
        template<class Key,class T> 
	void 		object (const char* name,const std::map<Key,T>& x);
	template<class T> 
	void		item (const std::string& id,const boost::shared_ptr<T> x);
	template<class T>
	void		vector (const char* name,const retin::auto_array_ptr<T>& x,int dim);
	template<class T> 
	void		vector (const char* name,const std::vector<T>& x,int dim);
	template<class T>
	void		matrix (const char* name,const retin::auto_array_ptr<T>& x,int dim,int n);
	template<class T>
	void		matrix (const char* name,const std::vector<T>& x,int dim,int n);
};

template<>
inline void	serialize (BinaryOutputSerializer& s,const jbyte& x) { s.stream().write((const char*)&x,sizeof(jbyte)); } 
template<>
inline void	serialize (BinaryOutputSerializer& s,const jint& x) { s.stream().write((const char*)&x,sizeof(jint)); } 
template<>
inline void	serialize (BinaryOutputSerializer& s,const jlong& x) { s.stream().write((const char*)&x,sizeof(jlong)); }
template<>
inline void	serialize (BinaryOutputSerializer& s,const jfloat& x) { s.stream().write((const char*)&x,sizeof(jfloat)); } 
template<>
inline void	serialize (BinaryOutputSerializer& s,const jdouble& x) { s.stream().write((const char*)&x,sizeof(jdouble)); } 
template<>
inline void	serialize (BinaryOutputSerializer& s,const std::string& str)
{
	uint n = str.size();
	if (n >= 256)
		retinThrowException1 ("String %s too large",str.c_str());
	s.stream().write ((const char*)&n,1);
	s.stream().write (&str[0],n);
}

template<class T>
void	BinaryOutputSerializer::object (const char* name,const T& x)
{
	serialize (*this,x);
}

template<class T>
void	BinaryOutputSerializer::object (const char* name,const boost::shared_ptr<T> x)
{
	if (!x)
	{
		writeString ("void");
		return;
	}
	writeString (x->className());
	x->serializeOut (*this);
}

template<class T>
void	BinaryOutputSerializer::object (const char* name,const std::vector<T>& x)
{
	uint n = x.size();
	s.write((const char*)&n,4);
	for (uint i=0;i<x.size();i++)
		serialize (*this,x[i]);
}

template<class T>
void	BinaryOutputSerializer::object (const char* name,const std::vector<boost::shared_ptr<T> >& x)
{
	uint n = x.size();
	s.write((const char*)&n,4);
	for (uint i=0;i<x.size();i++)
	{
		if (!x[i])
		{
			writeString ("void");
			continue;
		}
		writeString (x[i]->className());
		x[i]->serializeOut (*this);
	}
}

template<class Key,class T>
void	BinaryOutputSerializer::object (const char* name,const std::map<Key,boost::shared_ptr<T> >& x)
{
	uint n = x.size();
	s.write((const char*)&n,4);
	for (typename std::map<Key,boost::shared_ptr<T> >::const_iterator i=x.begin(),e=x.end();i!=e;i++)
	{
		serialize (*this,i->first);
		if (!i->second)
		{
			writeString ("void");
			continue;
		}
		writeString (i->second->className());
		i->second->serializeOut (*this);
	}
}

template<class Key,class T>
void	BinaryOutputSerializer::object (const char* name,const std::map<Key,T>& x)
{
	uint n = x.size();
	s.write((const char*)&n,4);
	for (typename std::map<Key,T>::const_iterator i=x.begin(),e=x.end();i!=e;i++)
	{
		serialize (*this,i->first);
		if (!i->second)
		{
			writeString ("void");
			continue;
		}
                serialize (*this,i->second);
	}
}

template<class T>
void	BinaryOutputSerializer::item (const std::string& id,const boost::shared_ptr<T> x)
{
	serialize (*this,id);
	if (!x)
	{
		writeString ("void");
		return;
	}
	writeString (x->className());
	x->serializeOut (*this);
}

template<class T>
void	BinaryOutputSerializer::vector (const char* name,const std::vector<T>& x,int dim)
{
	writeBuffer ((const char*)&x[0],(size_t)dim*sizeof(T));
}

template<class T>
void	BinaryOutputSerializer::vector (const char* name,const retin::auto_array_ptr<T>& x,int dim)
{
	writeBuffer ((const char*)x.get(),(size_t)dim*sizeof(T));
}

template<class T>
void	BinaryOutputSerializer::matrix (const char* name,const retin::auto_array_ptr<T>& x,int dim,int n)
{
	writeBuffer ((const char*)x.get(),(size_t)dim*(size_t)n*sizeof(T));
}

template<class T>
void	BinaryOutputSerializer::matrix (const char* name,const std::vector<T>& x,int dim,int n)
{
	writeBuffer ((const char*)&x[0],(size_t)dim*(size_t)n*sizeof(T));
}

}

#endif

