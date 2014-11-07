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
 * \file JNIOutputSerializer.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_JNIOutputSerializer_h__
#define __retin_JNIOutputSerializer_h__

#include "retin/toolbox/document/serialization/Java.h"
#include <map>

namespace retin
{

class	JNIOutputSerializer
{
	JavaEnv&	je;
	jobject		obj;
	jclass		objClass;

public:
	JNIOutputSerializer (JavaEnv& je,jobject obj);

	template<class T>
	void		object (const char* name,const T& x)
	{
		je.setField (obj,name,x);
	}

	template<class T>
	void		object (const char* name,const boost::shared_ptr<T>& x)
	{
		je.setDocumentField (obj,name,x);
	}

	template<class T>
	void		object (const char* name,const std::vector<T>& x)
	{
		retinThrowException("Non implanté");
	}

	template<class T>
	void		object (const char* name,const std::vector<boost::shared_ptr<T> >& x)
	{
		je.setDocumentField (obj,name,x);
	}
	
	template<class Key,class T>
	void		object (const char* name,const std::map<Key,boost::shared_ptr<T> >& x)
	{
		je.setDocumentField (obj,name,x);
	}
        
        template<class Key,class T>
	void		object (const char* name,const std::map<Key,T>& x)
	{
		retinThrowException("Non implanté");
	}

	template<class T>
	void		vector (const char* name,const std::vector<T>& x,int dim)
	{
		je.setArrayField(obj,name,x);
	}

	template<class T>
	void		vector (const char* name,const retin::auto_array_ptr<T>& x,int dim)
	{
		je.setArrayField(obj,name,x);
	}

        template<class T>
	void		matrix (const char* name,const retin::auto_array_ptr<T>& x,int dim,int n)
	{
		je.setArrayField (obj,name,x);
	}

	template<class T>
	void		matrix (const char* name,const std::vector<T>& x,int dim,int n)
	{
		je.setArrayField (obj,name,x);
	}
};

}

#endif
