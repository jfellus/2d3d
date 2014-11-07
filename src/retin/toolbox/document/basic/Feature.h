/**
 * \file Feature.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_Feature_h__
#define __retin_Feature_h__

#include "../Document.h"

#include <vector>

namespace retin
{

class	Feature : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/basic,Feature);

	jint			nDim;
	std::vector<jfloat>	vData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, Feature& x)
	{
		s.object ("dim",x.nDim);
		s.vector ("data",x.vData,x.nDim);
	}

public:
	Feature (int nDim=1) : nDim(nDim),vData(nDim) { }
	Feature (jfloat* data,int nDim) : nDim(nDim),vData(data,data+nDim) { }

	virtual bool	equals(const Document* doc) const;

	jint		dim() const { return nDim; }

	void		resize(int p)
	{
		nDim = p;
		vData.resize (p); 
	}

        void            setValue(int k,float x) {
 		BOOST_ASSERT (k >= 0 && k < nDim);
		vData[k] = x;
        }


	float*		data () { return &vData[0]; }

	jfloat&		operator[] (int k)
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return vData[k];
	}
	const jfloat&	operator[] (int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return vData[k];
	}
	jfloat&		operator() (int k)
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return vData[k];
	}
	const jfloat&	operator() (int k) const
	{
		BOOST_ASSERT (k >= 0 && k < nDim);
		return vData[k];
	}

	void		show() const;

};

typedef boost::shared_ptr<Feature> FeatureRef;

}

#endif

