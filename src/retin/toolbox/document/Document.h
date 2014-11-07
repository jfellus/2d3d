/**
 * \file Document.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Document_h__
#define __retin_Document_h__

#include "retin/toolbox/document/serialization/Serializable.h"

#include <boost/shared_ptr.hpp>


namespace retin
{

class	Document : public Serializable
{
public:
        static const char*      staticPackageName() { return "retin/toolbox/document"; }
	static const char*	staticClassName() { return "Document"; }
	static const char*	staticFullClassName() { return "retin/toolbox/document/Document"; }
        static const char*      staticFullJniClassName() { return "Lretin/toolbox/document/Document;"; }

	virtual bool	equals(const Document*) const { return false; }
	
	template<class T>
	bool		isInstanceOf () const
	{ 
		return strcmp(className(),T::staticClassName()) == 0;
	}

	template<class T>
	T*		as () 
	{ 
		if (!isInstanceOf<T>())
			retinThrowException2 ("Document has class %s, %s is expected",className(),T::staticClassName());
		return static_cast<T*>(this);
	}
};

typedef boost::shared_ptr<Document>	DocumentRef;

template<class T>
boost::shared_ptr<T>	documentAs (DocumentRef doc) 
{
        if (!doc)
                retinThrowException ("Document is null");
	if (strcmp(doc->className(),T::staticClassName()) != 0)
		retinThrowException2 ("Document has class %s, %s is expected",doc->className(),T::staticClassName());
	return boost::static_pointer_cast<T>(doc);
}

}

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_toolbox_document_Document_toXMLString
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_document_Document_fromXMLString
  (JNIEnv *, jobject, jobject);

JNIEXPORT jobject JNICALL Java_retin_toolbox_document_Document_makeXMLString
  (JNIEnv *, jobject, jobject);


}

#endif

