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
