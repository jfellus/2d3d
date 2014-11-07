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
