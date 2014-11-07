/**
 * \file IndexesList.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_IndexesList_h__
#define __retin_IndexesList_h__

#include "../Document.h"

#include <vector>

namespace retin
{

class	IndexesList : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/list,IndexesList);

	jint			nDim,nMaxb,nSize;
	std::vector<jint>	vData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, IndexesList& x)
	{
		s.object ("dim",x.nDim);
		s.object ("maxb",x.nMaxb);
		s.object ("size",x.nSize);
		s.matrix ("indexes",x.vData,x.nDim,x.nSize);
	}

public:
	IndexesList (int nDim=0,int nMaxb=0,int nSize=0) : nDim(nDim),nMaxb(nMaxb),nSize(nSize),vData(nMaxb*nSize) { }

	jint*		data() { return &vData[0]; }
	jint		size() const { return nSize; }
	jint		dim() const { return nDim; }
        jint            maxBinValue() const { return nMaxb; }

	void		setMaxBinValue (jint n) { nMaxb = n; }

	void		resize(int n,int m) 
	{
		nDim = n;
		nSize = m;
		vData.resize (n*m); 
	}

	jint&		operator() (int i,int j)
	{
		BOOST_ASSERT (i >= 0 && i < nDim);
		BOOST_ASSERT (j >= 0 && j < nSize);
		return vData[i+j*nDim];
	}
	const jint&	operator() (int i,int j) const
	{
		BOOST_ASSERT (i >= 0 && i < nDim);
		BOOST_ASSERT (j >= 0 && j < nSize);
		return vData[i+j*nDim];
	}

	jint&		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nDim*nSize);
		return vData[i];
	}
	const jint&	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nDim*nSize);
		return vData[i];
	}
};

typedef boost::shared_ptr<IndexesList> IndexesListRef;

}

#endif

