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

