/**
 * \file SerializableObjectFactory.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_SerializableObjectFactory_h__
#define __retin_SerializableObjectFactory_h__

#include "Serializable.h"
#include "ObjectFactory.h"

namespace retin {


class	AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const	= 0;
virtual ~AbstractSerializableCreator() {}
};

template<class T>
class	SerializableCreator : public AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const {
        SerializableRef doc = boost::make_shared<T>();
        if (!doc.get())
            retinThrowException1 ("Error while creating instance of %s",T::staticClassName());
        doc->fromJavaObject (je,obj);
        return doc;
    }
};

class SerializableObjectFactory : public ObjectFactory<Serializable>
{
    std::map<std::string,AbstractSerializableCreator*>	serializableCreators;

public:
    SerializableObjectFactory();
    ~SerializableObjectFactory();

    template<class T>
    void			registerClass (AbstractSerializableCreator* creator=NULL)
    {
        ObjectFactory<Serializable>::registerClass<T>(T::staticClassName());
        typename std::map<std::string,AbstractSerializableCreator*>::iterator i = serializableCreators.find (T::staticFullClassName());
        if (i != serializableCreators.end())
        {
                delete i->second;
                i->second = NULL;
        }
        if (!creator) creator = new SerializableCreator<T>();
        serializableCreators[T::staticFullClassName()] = creator;
    }

    template<class T>
    boost::shared_ptr<T>    createSerializable (JavaEnv& je,const std::string& fullClassName,jobject obj) {
        typename std::map<std::string,AbstractSerializableCreator*>::const_iterator i = serializableCreators.find (fullClassName);
        if (i == serializableCreators.end())
                retinThrowException1 ("Creator of %s not found",fullClassName.c_str());
        SerializableRef ptr = i->second->createSerializable(je,obj);
        if (!ptr.get())
                retinThrowException1 ("Error while creating instance of %s",fullClassName.c_str());
        boost::shared_ptr<T> x = boost::static_pointer_cast<T>(ptr);
        if (!x.get())
                retinThrowException2 ("Error while casting from %s to %s",fullClassName.c_str(),T::staticFullClassName());
        return x;
    }

};


typedef	boost::shared_ptr<SerializableObjectFactory>	SerializableObjectFactoryRef;

SerializableObjectFactoryRef getDefaultSerializableObjectFactory();


}

#endif


