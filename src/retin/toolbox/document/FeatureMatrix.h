/**
 * \file FeatureMatrix.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_FeatureMatrix_h__
#define __retin_FeatureMatrix_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/core/auto_array_ptr.h"

namespace retin
{

class	FeatureMatrix : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,FeatureMatrix);
protected:
	retin::auto_array_ptr<jfloat>	vData;
	jint			nDim;
	jint			nWidth,nHeight;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, FeatureMatrix& x)
	{
		s.object ("dim",x.nDim);
		s.object ("width",x.nWidth);
		s.object ("height",x.nHeight);
		s.matrix ("features",x.vData,x.nDim,x.nWidth*x.nHeight);
	}

public:
	typedef	jfloat	value_type;

	FeatureMatrix (int nWidth=0,int nHeight=0,int nDim=1) : vData(nDim*nWidth*nHeight),nDim(nDim),nWidth(nWidth),nHeight(nHeight) { }
	FeatureMatrix (jfloat* ptr,int nWidth=0,int nHeight=0,int nDim=1) : vData(ptr,nDim*nWidth*nHeight),nDim(nDim),nWidth(nWidth),nHeight(nHeight) { }

	jfloat*		data() { return vData.get(); }
	jfloat*		data(uint j) { return vData.get(j*nWidth*nDim); }
	jfloat*		releaseData () { return vData.release(); }
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

	jfloat&		operator() (int i,int j,int k=0)
	{
		BOOST_ASSERT (i >= 0 && i < nWidth);
		BOOST_ASSERT (j >= 0 && j < nHeight);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return vData[k+nDim*(i+j*nWidth)];
	}
	const jfloat&	operator() (int i,int j,int k=0) const
	{
		BOOST_ASSERT (i >= 0 && i < nWidth);
		BOOST_ASSERT (j >= 0 && j < nHeight);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return vData[k+nDim*(i+j*nWidth)];
	}

	jfloat*		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nWidth*nHeight);
		return vData.get()+i*nDim;
	}
	const jfloat*	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nWidth*nHeight);
		return vData.get()+i*nDim;
	}
};

typedef boost::shared_ptr<FeatureMatrix> FeatureMatrixRef;

}

#endif

