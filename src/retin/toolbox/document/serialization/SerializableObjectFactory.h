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
