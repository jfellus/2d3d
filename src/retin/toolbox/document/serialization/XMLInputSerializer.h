/**
 * \file XMLInputSerializer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_XMLInputSerializer_h__
#define __retin_XMLInputSerializer_h__

#include "retin/toolbox/document/serialization/SerializableObjectFactory.h"
#include "retin/toolbox/core/auto_array_ptr.h"


namespace retin
{

class	XMLInputSerializer
{
	int				line;
	std::istream&			s;
	SerializableObjectFactoryRef	factoryRef;
	
	void	skipSpaces ();
	void	expect (const char c);
	void	expect (const char* pc);

public:
	XMLInputSerializer (std::istream& s,SerializableObjectFactoryRef factory=SerializableObjectFactoryRef()) : line(1),s(s),factoryRef(factory)
	{ 
		if (!factoryRef)
			factoryRef = getDefaultSerializableObjectFactory();
	}

	void	throwException (const std::string& func,const std::string& msg);

	std::istream&	stream() { return s; }
	SerializableObjectFactoryRef	factory() { return factoryRef; }

	bool	eof();
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
	void		object (const char* name,std::map<Key,T>& x);
	template<class Key,class T>
	void		object (const char* name,std::map<Key,boost::shared_ptr<T> >& x);
	template<class T>
	std::string	item (boost::shared_ptr<T>& x);
	template<class T>
	void		vector (const char* name,std::vector<T>& x,int dim);
	template<class T>
	void		vector (const char* name,retin::auto_array_ptr<T>& x,int dim);
	template<class T>
	void		matrix (const char* name,retin::auto_array_ptr<T>& x,int dim,int n);
	template<class T>
	void		matrix (const char* name,std::vector<T>& x,int dim,int n);

};

template<>
inline void	serialize (XMLInputSerializer& s,jbyte& x) { int i; s.stream() >> i; x = (jbyte)i; }
template<>
inline void	serialize (XMLInputSerializer& s,jint& x) { s.stream() >> x; }
template<>
inline void	serialize (XMLInputSerializer& s,jlong& x) { s.stream() >> x; }
template<>
inline void	serialize (XMLInputSerializer& s,jfloat& x) { s.stream() >> x; }
template<>
inline void	serialize (XMLInputSerializer& s,jdouble& x) { s.stream() >> x; }
template<>
inline void	serialize (XMLInputSerializer& s,std::string& x)
{
	x.clear();
	std::istream& stream = s.stream();
	while (1)
	{
		uint c = stream.peek();
		if (c == '<' || c == '"')
			break;
		x += stream.get();
	}
}

template<class T>
void	XMLInputSerializer::object (const char* name,T& x)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	serialize(*this,x);
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class T>
void	XMLInputSerializer::object (const char* name,boost::shared_ptr<T>& x)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect(" class=\"");
	std::string className;
	serialize (*this,className);
	if (className == "void")
	{
		x.reset ();
		expect("\"/>");
		return;
	}
	expect("\">");
	x = factoryRef->createShared<T>(className);
	x->serializeIn (*this);
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class T>
void	XMLInputSerializer::object (const char* name,std::vector<T>& x)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	x.clear();
	while (true)
	{
		skipSpaces ();
		expect('<');
		if (s.peek() == '/')
		{
			s.unget ();
			break;
		}
		expect("item>");
		T item;
		serialize (*this,item);
		x.push_back (item);
		skipSpaces ();
		expect("</item>");
	}
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class T>
void	XMLInputSerializer::object (const char* name,std::vector<boost::shared_ptr<T> >& x)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	x.clear();
	std::string	className;
	while (true)
	{
		skipSpaces ();
		expect('<');
		if (s.peek() == '/')
		{
			s.unget ();
			break;
		}
		expect("item class=\"");
		serialize (*this,className);
		if (className == "void")
		{
			x.push_back (boost::shared_ptr<T>());
			expect("\"/>");
			continue;
		}
		expect("\">");
		boost::shared_ptr<T> doc = factoryRef->createShared<T>(className);
		doc->serializeIn (*this);
		x.push_back (doc);
		skipSpaces ();
		expect("</item>");
	}
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class Key,class T>
void	XMLInputSerializer::object (const char* name,std::map<Key,boost::shared_ptr<T> >& x)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	x.clear();
	std::string	className;
	Key		id;
	while (true)
	{
		skipSpaces ();
		expect('<');
		if (s.peek() == '/')
		{
			s.unget ();
			break;
		}
		expect("item id=\"");
		serialize (*this,id);
		expect("\" class=\"");
		serialize (*this,className);
		if (className == "void")
		{
			x.insert ( std::make_pair(id,boost::shared_ptr<T>()) );
			expect("\"/>");
			continue;
		}
		expect("\">");
		boost::shared_ptr<T> doc = factoryRef->createShared<T>(className);
		doc->serializeIn (*this);
		x.insert ( make_pair(id,doc) );
		skipSpaces ();
		expect("</item>");
	}
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class Key,class T>
void	XMLInputSerializer::object (const char* name,std::map<Key,T>& x)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	x.clear();
	Key		id;
	while (true)
	{
		skipSpaces ();
		expect('<');
		if (s.peek() == '/')
		{
			s.unget ();
			break;
		}
		expect("item id=\"");
		serialize (*this,id);
		expect("\">");
		T item;
		serialize (*this,item);
		x.insert ( std::make_pair(id,item) );
		skipSpaces ();
		expect("</item>");
	}
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class T>
std::string	XMLInputSerializer::item (boost::shared_ptr<T>& x)
{
	skipSpaces ();
	expect("<item id=\"");
	std::string id;
	serialize (*this,id);
	expect("\" class=\"");
	std::string className;
	serialize (*this,className);
	if (className == "void")
	{
		x.reset ();
		expect("\"/>");
		return id;
	}
	expect("\">");
	x = factoryRef->createShared<T>(className);
	x->serializeIn (*this);
	skipSpaces ();
	expect("</item>");
	return id;
}

template<class T>
void	XMLInputSerializer::vector (const char* name,std::vector<T>& x,int dim)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	skipSpaces ();
	x.resize (dim);
	for (int k=0;k<dim;k++)
		serialize (*this,x[k]);
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class T>
void	XMLInputSerializer::vector (const char* name,retin::auto_array_ptr<T>& x,int dim)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	skipSpaces ();
	x.resize (dim);
	for (int k=0;k<dim;k++)
		serialize (*this,x[k]);
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class T>
void	XMLInputSerializer::matrix (const char* name,retin::auto_array_ptr<T>& x,int dim,int n)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	x.resize (dim*n);
	for (int i=0;i<n;i++)
	{
		skipSpaces ();
		expect("<vector>");
		for (int k=0;k<dim;k++)
			serialize (*this,x[k+i*dim]);
		skipSpaces ();
		expect("</vector>");
	}
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}

template<class T>
void	XMLInputSerializer::matrix (const char* name,std::vector<T>& x,int dim,int n)
{
	skipSpaces ();
	expect('<');
	expect(name);
	expect('>');
	x.resize (dim*n);
	for (int i=0;i<n;i++)
	{
		skipSpaces ();
		expect("<vector>");
		for (int k=0;k<dim;k++)
			serialize (*this,x[k+i*dim]);
		skipSpaces ();
		expect("</vector>");
	}
	skipSpaces ();
	expect('<');
	expect('/');
	expect(name);
	expect('>');
}



}

#endif

