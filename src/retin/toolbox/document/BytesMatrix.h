/**
 * \file BytesMatrix.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_BytesMatrix_h__
#define __retin_BytesMatrix_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/core/auto_array_ptr.h"

namespace retin
{

class	BytesMatrix : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,BytesMatrix);

	retin::auto_array_ptr<jbyte>	vData;
	jint			nDim;
	jint			nWidth,nHeight;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, BytesMatrix& x)
	{
		s.object ("dim",x.nDim);
		s.object ("width",x.nWidth);
		s.object ("height",x.nHeight);
		s.matrix ("bytes",x.vData,x.nDim,x.nWidth*x.nHeight);
	}

public:
	typedef	uchar	value_type;

	BytesMatrix (int nWidth=0,int nHeight=0,int nDim=1) : vData(nDim*nWidth*nHeight),nDim(nDim),nWidth(nWidth),nHeight(nHeight) { }
	BytesMatrix (jbyte* ptr,int nWidth=0,int nHeight=0,int nDim=1,bool bOwnData=false) : vData(ptr,nDim*nWidth*nHeight,bOwnData),nDim(nDim),nWidth(nWidth),nHeight(nHeight) { }

	virtual bool	equals(const Document* doc) const;

	uchar*		data() const { return (uchar*)vData.get(); }
	uchar*		data(int i) { return (uchar*)vData.get()+i*nDim; }
        uchar*          releaseData() { return (uchar*)vData.release(); }
	jint		width() const { return nWidth; }
	jint		height() const { return nHeight; }
	jint		dim() const { return nDim; }

	void		resize(int n,int m,int p) 
	{
		nWidth = n;
		nHeight = m;
		nDim = p;
		vData.resize (n*m*p); 
	}

	uchar&		operator() (int i,int j,int k=0)
	{
		BOOST_ASSERT (i >= 0 && i < nWidth);
		BOOST_ASSERT (j >= 0 && j < nHeight);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return data()[k+nDim*(i+j*nWidth)];
	}
	const uchar&	operator() (int i,int j,int k=0) const
	{
		BOOST_ASSERT (i >= 0 && i < nWidth);
		BOOST_ASSERT (j >= 0 && j < nHeight);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return data()[k+nDim*(i+j*nWidth)];
	}

	uchar*		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nWidth*nHeight);
		return data()+i*nDim;
	}
	const uchar*	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nWidth*nHeight);
		return data()+i*nDim;
	}
};

typedef boost::shared_ptr<BytesMatrix> BytesMatrixRef;

}

#endif

