/**
 * \file FloatMatrix.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_FloatMatrix_h__
#define __retin_FloatMatrix_h__

#include "Document.h"
#include "../core/auto_array_ptr.h"

namespace retin
{

class	FloatMatrix : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,FloatMatrix);

	retin::auto_array_ptr<jfloat>	pData;
	jint				nVectorSize,nVectorCount;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, FloatMatrix& x)
	{
		s.object ("vectorSize",x.nVectorSize);
		s.object ("vectorCount",x.nVectorCount);
		s.matrix ("vectors",x.pData,x.nVectorSize,x.nVectorCount);
	}

public:
	FloatMatrix (jfloat* pData,int nVectorSize=1,int nVectorCount=1,bool bOwnData=false) :
		pData(pData,nVectorSize*nVectorCount,bOwnData),nVectorSize(nVectorSize),nVectorCount(nVectorCount) { }
	FloatMatrix (int nVectorSize=1,int nVectorCount=1) : 
		pData(nVectorSize*nVectorCount),nVectorSize(nVectorSize),nVectorCount(nVectorCount) { }

	jfloat*		data() { return pData.get(); }
	jfloat*		data(int j) { return pData.get()+j*nVectorSize; }
	void		setData (jfloat* p) { pData.set(p); }
	jint		vectorSize() const { return nVectorSize; }
	jint		vectorCount() const { return nVectorCount; }

	jfloat&		operator() (int i,int j=0)
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize);
		BOOST_ASSERT (j >= 0 && j < nVectorCount);
		return pData[i+j*nVectorSize];
	}
	const jfloat&	operator() (int i,int j=0) const
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize);
		BOOST_ASSERT (j >= 0 && j < nVectorCount);
		return pData[i+j*nVectorSize];
	}

	jfloat*		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize*nVectorCount);
		return pData.get()+i;
	}
	const jfloat*	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize*nVectorCount);
		return pData.get()+i;
	}
};

typedef boost::shared_ptr<FloatMatrix> FloatMatrixRef;

}

#endif

