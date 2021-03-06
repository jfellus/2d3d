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
 * \file FeatureList.h
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#ifndef __retin_FeatureList_h__
#define __retin_FeatureList_h__

#include "List.h"
#include "../basic/Feature.h"
#include "../../core/auto_array_ptr.h"

namespace retin
{

class	BytesList;

class	FeatureList : public List
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/list,FeatureList);

	jint				nDim;
	jint				nSize;
	retin::auto_array_ptr<jfloat>	pData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, FeatureList& x)
	{
		s.object ("dim",x.nDim);
		s.object ("size",x.nSize);
		s.matrix ("features",x.pData,x.nDim,x.nSize);
	}

public:
	FeatureList (jfloat* data,int nDim=1,int nSize=1,bool bOwnData=false) : nDim(nDim),nSize(nSize),pData(data,(size_t)nDim*(size_t)nSize,bOwnData) { }
	FeatureList (int nDim=1,int nSize=0) : nDim(nDim),nSize(nSize),pData((size_t)nDim*(size_t)nSize) {}
	FeatureList (const BytesList& list);

	jfloat*		data() { return pData.get(); }
	jfloat*		data(size_t i) { return pData.get(i*nDim); }
        const jfloat*	data(size_t i) const { return pData.get(i*nDim); }
	jfloat*		releaseData () { return pData.release(); }
virtual	jint		size() const { return nSize; }
virtual	jint		dim() const { return nDim; }

        void            addFeaturesList(FeatureList & featuresList, float perFeatures = 1.0, int maxFeatures = 0);
        
        void            subRandomFeatureList(float perFeatures, int maxFeatures);

	void		setData (jfloat* p) { pData.set(p); }

	void		resize(int p,int n,bool bKeepData=false);

	void		show (int i,int n) const;

        void            powerNorm(float power, std::string norm);
        
	template<class InputIterator>
	void		setRow(int k,InputIterator first,InputIterator last) {
		size_t i = 0;
		while(first != last) {
		    pData[k+size_t(i++)*nDim] = *first++;
		}
	}

	void		setFeature(int i,const float* data) {
		memcpy(pData.get(i*nDim),data,nDim*sizeof(float));
	}
	void		setFeature(int i,const double* data) {
                for(size_t k=0;k<(size_t)nDim;k++)
                    pData[k+i*nDim] = (float)data[k];
	}
	FeatureRef	getFeature(int i) {
		return boost::make_shared<Feature>(data(i),nDim);
	}
	void		addFeature(const float* data) {
		resize(nDim,nSize+1,true);
		memcpy(pData.get((nSize-1)*nDim),data,nDim*sizeof(float));
	}
        void            addFeatures(const float* data,size_t count) {
                resize(nDim,nSize+count,true);
		memcpy(pData.get((nSize-count)*nDim),data,count*nDim*sizeof(float));
        }
        
	jfloat&		operator() (int k,int i)
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pData[k+size_t(i)*nDim];
	}
	const jfloat&	operator() (int k,int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		BOOST_ASSERT (k >= 0 && k < nDim);
		return pData[k+size_t(i)*nDim];
	}

	jfloat*		operator[] (int i)
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		return &pData[size_t(i)*nDim];
	}
	const jfloat*	operator[] (int i) const
	{
		BOOST_ASSERT (i >= 0 && i < nSize);
		return &pData[size_t(i)*nDim];
	}
};

typedef boost::shared_ptr<FeatureList> FeatureListRef;

}

#endif
