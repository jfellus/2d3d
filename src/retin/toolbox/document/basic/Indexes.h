/**
 * \file Indexes.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Indexes_h__
#define __retin_Indexes_h__

#include "../Document.h"
#include "../../core/auto_array_ptr.h"

namespace retin
{

class	Indexes : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/basic,Indexes);

	jint			nDim,nLength;
	retin::auto_array_ptr<jint> vData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Indexes& x)
	{
		s.object ("dim",x.nDim);
		s.object ("length",x.nLength);
		s.vector ("data",x.vData,x.nLength);
	}

public:
	Indexes (int nDim=1,int nLength=0) : nDim(nDim),nLength(nLength),vData(nLength) { }
	Indexes (int nDim,const std::vector<jint>& d) : nDim(nDim),nLength(d.size()),vData(d) { }

	jint		dim() const { return nDim; }
	jint		length() const { return nLength; }

	void		resize(int p) { nLength = p; vData.resize (p); }
//	void		push_back (jint x) { vData.push_back(x); nLength ++; }

        void            show() const;

	jint*		data () { return &vData[0]; }
	jint*		releaseData () { return vData.release(); }

	jint&		operator[] (int k)
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}
	const jint&	operator[] (int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}
	jint&		operator() (int k)
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}
	const jint&	operator() (int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nLength);
		return vData[k];
	}

};

typedef boost::shared_ptr<Indexes> IndexesRef;

}

#endif

