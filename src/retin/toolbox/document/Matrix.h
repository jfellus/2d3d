/**
 * \file Matrix.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Matrix_h__
#define __retin_Matrix_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/core/auto_array_ptr.h"


namespace retin
{

class	Matrix : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,Matrix);

	retin::auto_array_ptr<jdouble>	pData;
	jint				nVectorSize,nVectorCount;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Matrix& x)
	{
		s.object ("vectorSize",x.nVectorSize);
		s.object ("vectorCount",x.nVectorCount);
		s.matrix ("vectors",x.pData,x.nVectorSize,x.nVectorCount);
	}

public:
	Matrix (jdouble* pData,int nVectorSize=1,int nVectorCount=1) :
		pData(pData,nVectorSize*nVectorCount),nVectorSize(nVectorSize),nVectorCount(nVectorCount) { }
	Matrix (int nVectorSize=1,int nVectorCount=1) : 
		pData(nVectorSize*nVectorCount),nVectorSize(nVectorSize),nVectorCount(nVectorCount) { }

	jdouble*	data() { return pData.get(); }
	jdouble*	data(size_t j) { return pData.get()+j*nVectorSize; }
	void		setData (jdouble* p) { pData.set(p); }
	jint		vectorSize() const { return nVectorSize; }
	jint		vectorCount() const { return nVectorCount; }

	void		resize(int nVectorSize=1,int nVectorCount=1);

	jdouble&		operator() (int i,int j=0)
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize);
		BOOST_ASSERT (j >= 0 && j < nVectorCount);
		return pData[i+j*nVectorSize];
	}
	const jdouble&	operator() (int i,int j=0) const
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize);
		BOOST_ASSERT (j >= 0 && j < nVectorCount);
		return pData[i+j*nVectorSize];
	}

	jdouble*		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize*nVectorCount);
		return pData.get()+i;
	}
	const jdouble*	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nVectorSize*nVectorCount);
		return pData.get()+i;
	}
};

typedef boost::shared_ptr<Matrix> MatrixRef;

}

#endif

