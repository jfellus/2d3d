/**
 * \file IndexMatrix.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_IndexMatrix_h__
#define __retin_IndexMatrix_h__

#include "retin/toolbox/document/Document.h"

#include <vector>

namespace retin
{

class	IndexMatrix : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,IndexMatrix);

	jint			nDim;
	jint			nWidth,nHeight;
	std::vector<jint>	vData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, IndexMatrix& x)
	{
		s.object ("dim",x.nDim);
		s.object ("width",x.nWidth);
		s.object ("height",x.nHeight);
		s.matrix ("indexes",x.vData,x.nWidth,x.nHeight);
	}

public:
	IndexMatrix (int nWidth=0,int nHeight=0,int nDim=0) : nDim(nDim),nWidth(nWidth),nHeight(nHeight),vData(nWidth*nHeight) { }

	jint*		data() { return &vData[0]; }
	jint		width() const { return nWidth; }
	jint		height() const { return nHeight; }
	jint		dim() const { return nDim; }

	void		setDim (jint n) { nDim = n; }

	void		resize(int n,int m) 
	{
		nWidth = n;
		nHeight = m;
		vData.resize (n*m); 
	}

	jint&		operator() (int i,int j)
	{
		BOOST_ASSERT (i >= 0 && i < nWidth);
		BOOST_ASSERT (j >= 0 && j < nHeight);
		return vData[i+j*nWidth];
	}
	const jint&	operator() (int i,int j) const
	{
		BOOST_ASSERT (i >= 0 && i < nWidth);
		BOOST_ASSERT (j >= 0 && j < nHeight);
		return vData[i+j*nWidth];
	}

	jint&		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nWidth*nHeight);
		return vData[i];
	}
	const jint&	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nWidth*nHeight);
		return vData[i];
	}
};

typedef boost::shared_ptr<IndexMatrix> IndexMatrixRef;

}

#endif

