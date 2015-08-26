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
