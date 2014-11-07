/**
 * \file XMLOutputSerializer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_XMLOutputSerializer_h__
#define __retin_XMLOutputSerializer_h__

#include "retin/toolbox/core/core.h"

#include <iostream>
#include <map>

namespace retin
{

class	XMLOutputSerializer
{
	int		level;
	bool		needSep;
	std::ostream&	s;

	void	indent () { for (int i=0;i<level;i++) s << '\t'; }

public:
	XMLOutputSerializer (std::ostream& s) : level(0),needSep(false),s(s) { }

	std::ostream&	stream() { return s; }
	void	header () { s << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<root>\n"; }
	void	footer () { s << "</root>\n"; }

	template<class T> 
	void		object (const char* name,const T& x);
	template<class T> 
	void		object (const char* name,const boost::shared_ptr<T> x);
	template<class T> 
	void		object (const char* name,const std::vector<T>& x);
	template<class T> 
	void		object (const char* name,const std::vector<boost::shared_ptr<T> >& x);
        template<class Key,class T> 
	void 		object (const char* name,const std::map<Key,T>& x);
	template<class Key,class T> 
	void 		object (const char* name,const std::map<Key,boost::shared_ptr<T> >& x);
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
inline void	serialize (XMLOutputSerializer& s,const jbyte& x) { s.stream() << (int)(uchar)x; } 
template<>
inline void	serialize (XMLOutputSerializer& s,const jint& x) { s.stream() << x; } 
template<>
inline void	serialize (XMLOutputSerializer& s,const jlong& x) { s.stream() << x; }
template<>
inline void	serialize (XMLOutputSerializer& s,const jfloat& x) { s.stream() << x; } 
template<>
inline void	serialize (XMLOutputSerializer& s,const jdouble& x) { s.stream() << x; } 
template<>
inline void	serialize (XMLOutputSerializer& s,const std::string& x) { s.stream() << x; } 

template<class T>
void		XMLOutputSerializer::object (const char* name,const T& x)
{
	indent (); s << '<' << name << '>';
	serialize(*this,x);
	s << "</" << name << ">\n";
}

template<class T>
void		XMLOutputSerializer::object (const char* name,const boost::shared_ptr<T> x)
{
	indent (); s << '<' << name;
	if (!x)
	{
		s << " class=\"void\"/>\n";
		return;
	}
	s << " class=\"" << x->className() << "\">\n";
	level ++;
	x->serializeOut (*this);
	level --;
	indent (); s << "</" << name << ">\n";
}

template<class T>
void		XMLOutputSerializer::object (const char* name,const std::vector<T>& x)
{
	indent (); s << '<' << name << ">\n";
	level ++;
	for (uint i=0;i<x.size();i++)
	{
		indent(); s << "<item>";
		level ++;
		serialize (*this,x[i]);
		level --;
		s << "</item>\n";
	}
	level --;
	indent (); s << "</" << name << ">\n";
}

template<class T>
void		XMLOutputSerializer::object (const char* name,const std::vector<boost::shared_ptr<T> >& x)
{
	indent (); s << '<' << name << ">\n";
	level ++;
	for (uint i=0;i<x.size();i++)
	{
		indent(); 
		if (!x[i])
		{
			s << "<item class=\"void\"/>\n";
			continue;
		}
		s << "<item class=\"" << x[i]->className() << "\">\n";
		level ++;
		x[i]->serializeOut (*this);
		level --;
		indent(); s << "</item>\n";
	}
	level --;
	indent (); s << "</" << name << ">\n";
}

template<class Key,class T>
void		XMLOutputSerializer::object (const char* name,const std::map<Key,T>& x)
{
	indent (); s << '<' << name << ">\n";
	level ++;
	for (typename std::map<Key,T>::const_iterator i=x.begin(),e=x.end();i!=e;i++)
	{
		indent(); s << "<item id=\"" << i->first << "\">\n";
		level ++;
		serialize (*this,i->second);
		level --;
		indent(); s << "</item>\n";
	}
	level --;
	indent (); s << "</" << name << ">\n";
}

template<class Key,class T>
void		XMLOutputSerializer::object (const char* name,const std::map<Key,boost::shared_ptr<T> >& x)
{
	indent (); s << '<' << name << ">\n";
	level ++;
	for (typename std::map<Key,boost::shared_ptr<T> >::const_iterator i=x.begin(),e=x.end();i!=e;i++)
	{
		indent(); s << "<item id=\"" << i->first;
		if (!i->second)
		{
			s << "\" class=\"void\"/>\n";
			continue;
		}
		s << "\" class=\"" << i->second->className() << "\">\n";
		level ++;
		i->second->serializeOut (*this);
		level --;
		indent(); s << "</item>\n";
	}
	level --;
	indent (); s << "</" << name << ">\n";
}

template<class T>
void		XMLOutputSerializer::item (const std::string& id,const boost::shared_ptr<T> x)
{
	indent (); s << "<item id=\"" << id << "\" class=\"";
	if (!x)
	{
		s <<"void\"/>\n";
		return;
	}
	s << x->className() << "\">\n";
	level ++;
	x->serializeOut (*this);
	level --;
	indent (); s << "</item>\n";
}


template<class T>
void		XMLOutputSerializer::vector (const char* name,const std::vector<T>& x,int dim)
{
	indent (); s << '<' << name << ">";
	for (int k=0;k<dim;k++)
	{
		serialize(*this,x[k]); 
		s << ' ';
	}
	s << "</" << name << ">\n";
}


template<class T>
void		XMLOutputSerializer::vector (const char* name,const retin::auto_array_ptr<T>& x,int dim)
{
	indent (); s << '<' << name << ">";
	for (int k=0;k<dim;k++)
	{
		serialize(*this,x[k]);
		s << ' ';
	}
	s << "</" << name << ">\n";
}

template<class T>
void		XMLOutputSerializer::matrix (const char* name,const retin::auto_array_ptr<T>& x,int dim,int n)
{
	indent (); s << '<' << name << ">\n";
	level ++;
	for (int i=0;i<n;i++)
	{
		indent ();
		s << "<vector>";
		for (int k=0;k<dim;k++)
		{
			serialize(*this,x[k+i*dim]);
			s << ' ';
		}
		s << "</vector>\n";
	}
	level --;
	indent(); s << "</" << name << ">\n";
}

template<class T>
void		XMLOutputSerializer::matrix (const char* name,const std::vector<T>& x,int dim,int n)
{
	indent (); s << '<' << name << ">\n";
	level ++;
	for (int i=0;i<n;i++)
	{
		indent ();
		s << "<vector>";
		for (int k=0;k<dim;k++)
		{
			serialize(*this,x[k+i*dim]);
			s << ' ';
		}
		s << "</vector>\n";
	}
	level --;
	indent(); s << "</" << name << ">\n";
}


}

#endif

