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
 * \file ObjectFactory.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_ObjectFactory_h__
#define __retin_ObjectFactory_h__

#include "retin/toolbox/core/core.h"
#include <vector>
#include <map>

namespace retin
{

template<class BaseType>
class	AbstractCreator
{
public:
virtual BaseType*			create () const 	= 0;
virtual boost::shared_ptr<BaseType>	createShared () const 	= 0;
virtual ~AbstractCreator () {}
};

template<class BaseType,class T>
class	Creator : public AbstractCreator<BaseType>
{
public:
virtual BaseType*			create () const 	{ return new T; }
virtual boost::shared_ptr<BaseType>	createShared () const 	{ return boost::make_shared<T>(); }
};

template<class BaseType>
class	ObjectFactory
{
	std::map<std::string,AbstractCreator<BaseType>*>	creators;

public:
virtual	~ObjectFactory ()
	{
		for (typename std::map<std::string,AbstractCreator<BaseType>*>::iterator i=creators.begin(),e=creators.end();i!=e;i++)
			if (i->second)
				delete i->second;
	}

	void	getRegisteredClassNames (std::vector<std::string>& list) const 
	{
		for (typename std::map<std::string,AbstractCreator<BaseType>*>::const_iterator i=creators.begin(),e=creators.end();i!=e;i++)
			list.push_back(i->first);
	}

	template<class T>
	void			registerClass (const std::string& className)
	{
		typename std::map<std::string,AbstractCreator<BaseType>*>::iterator i = creators.find (className);
		if (i != creators.end())
		{
			delete i->second;
			i->second = NULL;
		}
		creators[className] = new Creator<BaseType,T>();
	}

	template<class T>
	boost::shared_ptr<T>	createShared (const std::string& className) const
	{
		typename std::map<std::string,AbstractCreator<BaseType>*>::const_iterator i = creators.find (className);
		if (i == creators.end())
			retinThrowException1 ("Creator of %s not found",className.c_str());
                boost::shared_ptr<BaseType> ptr = i->second->createShared();
                if (!ptr.get())
			retinThrowException1 ("Error while creating instance of %s",className.c_str());
                boost::shared_ptr<T> x = boost::static_pointer_cast<T>(ptr);
                if (!x.get())
			retinThrowException2 ("Error while casting from %s to %s",className.c_str(),T::staticClassName());
		return x;
	}

	template<class B>
	friend std::ostream&	operator<< (std::ostream& s,const ObjectFactory<B>& x)
	{
		for (typename std::map<std::string,AbstractCreator<BaseType>*>::const_iterator i=x.creators.begin(),e=x.creators.end();i!=e;i++)
			s << i->first << "\n";
		return s;
	}

};

}

#endif
