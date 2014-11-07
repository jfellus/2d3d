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

