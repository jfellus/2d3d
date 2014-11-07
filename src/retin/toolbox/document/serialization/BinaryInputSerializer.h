/**
 * \file BinaryInputSerializer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_BinaryInputSerializer_h__
#define __retin_BinaryInputSerializer_h__

#include "retin/toolbox/document/serialization/SerializableObjectFactory.h"
#include "retin/toolbox/core/auto_array_ptr.h"

namespace retin
{

class	BinaryInputSerializer
{
	std::istream&			s;
	SerializableObjectFactoryRef	factoryRef;

	void	readBuffer (char* buf,size_t n);

public:
	BinaryInputSerializer (std::istream& s,SerializableObjectFactoryRef factory=SerializableObjectFactoryRef()) : s(s),factoryRef(factory)
	{ 
		if (!factoryRef)
			factoryRef = getDefaultSerializableObjectFactory();
	}

	void	throwException (const std::string& func,const std::string& msg);

	std::istream&	stream() { return s; }
	SerializableObjectFactoryRef	factory() { return factoryRef; }

	bool	eof() { s.peek(); return s.eof(); }
	void	header ();
	void	footer ();
	
	template<class T>
	void		object (const char* name,T& x);
	template<class T> 
	void		object (const char* name,boost::shared_ptr<T>& x);
	template<class T>
	void		object (const char* name,std::vector<T>& x);
	template<class T>
	void		object (const char* name,std::vector<boost::shared_ptr<T> >& x);
	template<class Key,class T>
	void		object (const char* name,std::map<Key,boost::shared_ptr<T> >& x);
	template<class Key,class T>
	void		object (const char* name,std::map<Key,T>& x);
	template<class T>
	std::string	item (boost::shared_ptr<T>& x);
	template<class T>
	void		vector (const char* name,retin::auto_array_ptr<T>& x,int dim);
	template<class T>
	void		vector (const char* name,std::vector<T>& x,int dim);
	template<class T>
	void		matrix (const char* name,retin::auto_array_ptr<T>& x,int dim,int n);
	template<class T>
	void		matrix (const char* name,std::vector<T>& x,int dim,int n);

};

template<>
inline void	serialize (BinaryInputSerializer& s,jbyte& x) { s.stream().read ((char*)&x,sizeof(jbyte)); }
template<>
inline void	serialize (BinaryInputSerializer& s,jint& x) { s.stream().read ((char*)&x,sizeof(jint)); }
template<>
inline void	serialize (BinaryInputSerializer& s,jlong& x) { s.stream().read ((char*)&x,sizeof(jlong)); }
template<>
inline void	serialize (BinaryInputSerializer& s,jfloat& x) { s.stream().read ((char*)&x,sizeof(jfloat)); }
template<>
inline void	serialize (BinaryInputSerializer& s,jdouble& x) { s.stream().read ((char*)&x,sizeof(jdouble)); }
template<>
inline void	serialize (BinaryInputSerializer& s,std::string& x)
{
	uint n = 0;
	s.stream().read ((char*)&n,1);
	x.resize(n,' ');
	s.stream().read ((char*)&x[0],n);
}


template<class T>
void	BinaryInputSerializer::object (const char* name,T& x)
{
	serialize (*this,x);
}

template<class T>
void	BinaryInputSerializer::object (const char* name,boost::shared_ptr<T>& x)
{
	std::string className;
	serialize (*this,className);
	if (className == "void")
	{
		x.reset ();
		return;
	}
	x = factoryRef->createShared<T>(className);
	x->serializeIn (*this);
}

template<class T>
void	BinaryInputSerializer::object (const char* name,std::vector<T>& x)
{
	x.clear();
	uint n = 0;
	s.read ((char*)&n,4);
	T item;
	for (uint i=0;i<n;i++)
	{
		serialize (*this,item);
		x.push_back (item);
	}
}

template<class T>
void	BinaryInputSerializer::object (const char* name,std::vector<boost::shared_ptr<T> >& x)
{
	x.clear();
	uint n = 0;
	s.read ((char*)&n,4);
	std::string className;
	for (uint i=0;i<n;i++)
	{
		serialize (*this,className);
		if (className == "void")
		{
			x.push_back (boost::shared_ptr<T>());
			continue;
		}
		boost::shared_ptr<T> doc = factoryRef->createShared<T>(className);
		doc->serializeIn (*this);
		x.push_back (doc);
	}
}

template<class Key,class T>
void	BinaryInputSerializer::object (const char* name,std::map<Key,boost::shared_ptr<T> >& x)
{
	x.clear();
	uint n = 0;
	s.read ((char*)&n,4);
	std::string 	className;
	Key		id;
	for (uint i=0;i<n;i++)
	{
		serialize (*this,id);
		serialize (*this,className);
		if (className == "void")
		{
			x.insert ( make_pair(id,boost::shared_ptr<T>()) );
			continue;
		}
		boost::shared_ptr<T> doc = factoryRef->createShared<T>(className);
		doc->serializeIn (*this);
		x.insert ( make_pair(id,doc) );
	}
}

template<class Key,class T>
void	BinaryInputSerializer::object (const char* name,std::map<Key,T>& x)
{
	x.clear();
	uint n = 0;
	s.read ((char*)&n,4);
	T item;
	Key		id;
	for (uint i=0;i<n;i++)
	{
		serialize (*this,id);
		serialize (*this,item);
		x.insert ( std::make_pair(id,item) );
	}
}

template<class T>
std::string	BinaryInputSerializer::item (boost::shared_ptr<T>& x)
{
	std::string id,className;
	serialize (*this,id);
	serialize (*this,className);
	if (className == "void")
	{
		x.reset ();
		return id;
	}
	x = factoryRef->createShared<T>(className);
	x->serializeIn (*this);
	return id;
}

template<class T>
void	BinaryInputSerializer::vector (const char* name,std::vector<T>& x,int dim)
{
	x.resize (dim);
	readBuffer ((char*)&x[0],(size_t)dim*sizeof(T));
}

template<class T>
void	BinaryInputSerializer::vector (const char* name,retin::auto_array_ptr<T>& x,int dim)
{
	x.resize (dim);
	readBuffer ((char*)x.get(),(size_t)dim*sizeof(T));
}

template<class T>
void	BinaryInputSerializer::matrix (const char* name,retin::auto_array_ptr<T>& x,int dim,int n)
{
	x.resize (dim*n);
	readBuffer ((char*)x.get(),(size_t)dim*(size_t)n*sizeof(T));
}
template<class T>
void	BinaryInputSerializer::matrix (const char* name,std::vector<T>& x,int dim,int n)
{
	x.resize (dim*n);
	readBuffer ((char*)&x[0],(size_t)dim*(size_t)n*sizeof(T));
}


}

#endif

