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
 * \file BytesList.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_BytesList_h__
#define __retin_BytesList_h__

#include "List.h"
#include "../../core/auto_array_ptr.h"


namespace retin
{

class	BytesList : public List
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/list,BytesList);

	jint				nDim;
	jint				nSize;
	retin::auto_array_ptr<jbyte>	pData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, BytesList& x)
	{
		s.object ("dim",x.nDim);
		s.object ("size",x.nSize);
		s.matrix ("bytes",x.pData,x.nDim,x.nSize);
	}

public:
	BytesList (jbyte* data,int nDim=1,int nSize=1) : nDim(nDim),nSize(nSize),pData(data,nDim*nSize) { }
	BytesList (int nDim=1,int nSize=1) : nDim(nDim),nSize(nSize),pData(nDim*nSize) { }

	uchar*		data() { return (uchar*)pData.get(); }
	const uchar*	data() const { return (uchar*)pData.get(); }
	uchar*		releaseData () { return (uchar*)pData.release(); }
virtual	jint		size() const { return nSize; }
virtual	jint		dim() const { return nDim; }

	void		setData (uchar* p) { pData.set((jbyte*)p); }

	void		resize(int p,int n,bool bKeepData=false);

	void		show (int i,int n) const;
        void            show () const { show(0,size()); }

	uchar&		operator() (int k,int i)
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return (uchar&)pData[k+size_t(i)*nDim];
	}
	const uchar&	operator() (int k,int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return (uchar&)pData[k+size_t(i)*nDim];
	}

	uchar*		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		return (uchar*)&pData[size_t(i)*nDim];
	}
	const uchar*	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		return (uchar*)&pData[size_t(i)*nDim];
	}
};

typedef boost::shared_ptr<BytesList> BytesListRef;

}

#endif
