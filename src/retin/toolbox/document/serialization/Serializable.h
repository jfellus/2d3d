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
