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
 * \file Indexes.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Indexes_h__
#define __retin_Indexes_h__

#include "../Document.h"
#include "../../core/auto_array_ptr.h"

namespace retin
{

class	Indexes : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/basic,Indexes);

	jint			nDim,nLength;
	retin::auto_array_ptr<jint> vData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Indexes& x)
	{
		s.object ("dim",x.nDim);
		s.object ("length",x.nLength);
		s.vector ("data",x.vData,x.nLength);
	}

public:
	Indexes (int nDim=1,int nLength=0) : nDim(nDim),nLength(nLength),vData(nLength) { }
	Indexes (int nDim,const std::vector<jint>& d) : nDim(nDim),nLength(d.size()),vData(d) { }

	jint		dim() const { return nDim; }
	jint		length() const { return nLength; }

	void		resize(int p) { nLength = p; vData.resize (p); }
//	void		push_back (jint x) { vData.push_back(x); nLength ++; }

        void            show() const;

	jint*		data () { return &vData[0]; }
	jint*		releaseData () { return vData.release(); }

	jint&		operator[] (int k)
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}
	const jint&	operator[] (int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}
	jint&		operator() (int k)
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}
	const jint&	operator() (int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}

};

typedef boost::shared_ptr<Indexes> IndexesRef;

}

#endif
