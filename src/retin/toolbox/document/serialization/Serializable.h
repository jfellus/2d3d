/**
 * \file Serializable.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Serializable_h__
#define __retin_Serializable_h__

#include "retin/toolbox/core/core.h"
#include <jni.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


#define RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(package,name) \
public: \
	static  const char*  	staticPackageName() { return #package; } \
	static  const char*  	staticClassName() { return #name; } \
	static  const char*  	staticFullClassName() { return #package"/"#name; } \
        static  const char*     staticFullJniClassName() { return "L"#package"/"#name";"; } \
	virtual const char*  	packageName() const { return #package; } \
	virtual const char*  	className() const { return #name; } \
	static  const char*  	fullClassName() { return #package"/"#name; } \
        virtual const char*     fullJniClassName() const { return "L"#package"/"#name";"; } \
	virtual void		serializeIn (XMLInputSerializer& s); \
	virtual void		serializeIn (BinaryInputSerializer& s); \
	virtual void		serializeIn (JNIInputSerializer& s); \
	virtual void		serializeOut (XMLOutputSerializer& s); \
	virtual void		serializeOut (JNIOutputSerializer& s); \
	virtual void		serializeOut (BinaryOutputSerializer& s); \
private:

#define RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(name) \
namespace retin { \
	void	name::serializeIn (XMLInputSerializer& s)   { return serialize(s,*this); } \
	void	name::serializeIn (BinaryInputSerializer& s)   { return serialize(s,*this); } \
	void	name::serializeIn (JNIInputSerializer& s)   { return serialize(s,*this); } \
	void	name::serializeOut (XMLOutputSerializer& s) { return serialize(s,*this); } \
	void	name::serializeOut (BinaryOutputSerializer& s) { return serialize(s,*this); } \
	void	name::serializeOut (JNIOutputSerializer& s) { return serialize(s,*this); } \
} 

namespace retin
{

class	JavaEnv;
class	XMLInputSerializer;
class	BinaryInputSerializer;
class	JNIInputSerializer;
class	XMLOutputSerializer;
class	BinaryOutputSerializer;
class	JNIOutputSerializer;

class	Serializable
{
public:
	virtual ~Serializable () { }
        virtual const char*     packageName() const = 0;
	virtual const char*  	className() const = 0;
        virtual const char*     fullJniClassName() const = 0;
	virtual void		serializeIn (XMLInputSerializer& s) = 0;
	virtual void		serializeIn (BinaryInputSerializer& s) = 0;
	virtual void		serializeIn (JNIInputSerializer& s) = 0;
	virtual void		serializeOut (XMLOutputSerializer& s) = 0;
	virtual void		serializeOut (BinaryOutputSerializer& s) = 0;
	virtual void		serializeOut (JNIOutputSerializer& s) = 0;

		std::string	toXMLString ();
		void		toJavaObject (JavaEnv& je,jobject obj);
		void		fromXMLString (const std::string& str);
		void		fromJavaObject (JavaEnv& je,jobject obj);
};

typedef boost::shared_ptr<Serializable> SerializableRef;


template<class SerializerType,class T>
void	serialize (SerializerType&, T&)
{
	throw std::runtime_error("Serialization non implantée");
}

}

#endif

