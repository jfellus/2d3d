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
