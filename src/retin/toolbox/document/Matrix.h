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
