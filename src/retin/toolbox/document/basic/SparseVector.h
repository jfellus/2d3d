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
/* 
 * File:   SparseVector.h
 * Author: romanegr
 *
 * Created on 8 juillet 2013, 11:15
 */

#ifndef __SPARSEVECTOR_H__
#define	__SPARSEVECTOR_H__

#include "../Document.h"
#include "Feature.h"

#include <map>
#include <algorithm>
#include <vector>
#include <math.h>

namespace retin
{

typedef std::map<jint,jfloat>::iterator SparseVector_iterator;
    
class	SparseVector : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document/basic,SparseVector);

	jint			nDim;
	std::map<jint,jfloat>	mData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, SparseVector& x)
	{
                jint nonZeroVal = x.mData.size();
		s.object ("dim", x.nDim);
                s.object ("nonZeroVal", nonZeroVal);
		s.object ("data", x.mData);
	}

public:
	SparseVector (jint nDim = 1) : nDim(nDim){}
        
        SparseVector (const jfloat * buffer, const jint nDim, const jfloat threshold = 0.0f) : nDim(nDim){
            for(jint i = 0 ; i < nDim ; i++){
                if(fabs(buffer[i]) > fabs(threshold))
                        mData.insert( std::pair<jint,jfloat>(i,buffer[i]));
            }
        }
        
        SparseVector (const SparseVector& o) : nDim(o.nDim), mData(o.mData){
        }
        
        virtual ~SparseVector (){}
        
        virtual bool	equals(const Document* doc) const;
        
        jint		dim() const { return nDim; }
        jint		nbNonZeroVal() const { return mData.size(); }
        
        jfloat          sparsityRate() const {
                return ((jfloat)nbNonZeroVal())/((jfloat)dim());
        }
        
        void            setValue(jint k,jfloat x) {
 		BOOST_ASSERT (k >= 0 && k < nDim);
		mData.insert ( std::pair<jint,jfloat>(k, x) );
        }
        
        jfloat          getValue(jint k) {
 		BOOST_ASSERT (k >= 0 && k < nDim);
                SparseVector_iterator it = mData.find(k);
                if(it == mData.end())
                    return 0.0f;
                return it->second;
        }
        
        void           setVector(const jfloat * buffer, const jint nDim, const jfloat threshold = 0.0f){
                mData.clear();
                this->nDim = nDim;
                for(jint i = 0 ; i < nDim ; i++){
                    if(fabs(buffer[i]) > fabs(threshold)){
                        mData.insert( std::pair<jint,jfloat>(i,buffer[i]));
                    }
                }
        }
        
        jfloat         compDotProd(const jfloat * vect, jint n){
                BOOST_ASSERT (n == nDim);
                float res = 0.0f;
                for(SparseVector_iterator it=mData.begin(); it!=mData.end(); ++it)
                        res += vect[it->first] * it->second;
                return res;
        }
        
        jfloat         compDotProd(FeatureRef f){
                return compDotProd(f->data(),f->dim());
        }
        
        void           clear(){
                mData.clear();
        }
        
        void		resize(int p)
	{
                BOOST_ASSERT (p > 0);
                if(nDim < p)
                        nDim = p;
                else {
                    mData.erase(mData.lower_bound(p), mData.end());
                }
	}
        
        void            hardThresholding(const jfloat threshold, size_t minNonZeroVal = 0){
                BOOST_ASSERT (minNonZeroVal >= 0);
                std::map<jint,jfloat>	temp;
                for(SparseVector_iterator it=mData.begin(); it!=mData.end(); ++it){
                    if(fabs(it->second) >= fabs(threshold))
                        temp.insert( std::pair<jint,jfloat>(it->first,it->second));
                }
                mData = temp;
        }
        
        void            softThresholding(const jfloat threshold, size_t minNonZeroVal = 0){
                BOOST_ASSERT (minNonZeroVal >= 0);
                std::map<jint,jfloat>	temp;
                for(SparseVector_iterator it=mData.begin(); it!=mData.end(); ++it){
                    if(fabs(it->second) > fabs(threshold)){
                        jfloat val = it->second;
                        if(val>0.0f)
                            val -= fabs(threshold);
                        else
                            val += fabs(threshold);
                        temp.insert( std::pair<jint,jfloat>(it->first,val));
                    }
                }
                mData = temp;
        }
        
        void            removesSmallerValues(size_t nonZeroVal, bool softThreshol = false ){
                if(nonZeroVal >= mData.size())
                    return;
                std::vector<jfloat> temp_sort(mData.size());
                size_t i = 0;
                for(SparseVector_iterator it=mData.begin(); it!=mData.end(); ++it){
                    temp_sort[i++] = fabs(it->second);
                }
                std::sort (temp_sort.begin(), temp_sort.end());
                if(softThreshol)
                    softThresholding(temp_sort[temp_sort.size()-nonZeroVal], nonZeroVal);
                else
                        hardThresholding(temp_sort[temp_sort.size()-nonZeroVal], nonZeroVal);
        }
        
        void            removesSmallerValues(jfloat pNonZeroVal, bool softThreshol = false){
                BOOST_ASSERT (pNonZeroVal > 0.0f && pNonZeroVal <= 1.0f);
                removesSmallerValues((size_t)(pNonZeroVal*nDim), softThreshol);
        }
        
        void            linear(jfloat * out, const jfloat * in, jfloat alpha, jint n){
                BOOST_ASSERT (n == nDim);
                if(out != in)
                    memcpy(out,in,sizeof(float)*n);
                
                for(SparseVector_iterator it=mData.begin(); it!=mData.end(); ++it)
                        out[it->first] += alpha * it->second;
        }
        
        void         add_mul_vector(jfloat * v, jfloat mul, jfloat pNonZeroVal, jint n, bool softThreshol = false){
                BOOST_ASSERT (n == nDim);
                std::vector<jfloat> temp(nDim,0);
                for(SparseVector_iterator it=mData.begin(); it!=mData.end(); ++it)
                        temp[it->first] = it->second;
                
                for(jint i = 0 ; i < nDim ; i++){
                    temp[i] += mul*v[i];
                }
                setVector(&temp[0],n);
                removesSmallerValues(pNonZeroVal, softThreshol);
        }
        
        jfloat       norml2(){
            return sqrt(norml2p2());
        }
        
        jfloat       norml2p2(){
                jfloat n = 0.0f;
                for(SparseVector_iterator it=mData.begin(); it!=mData.end(); ++it)
                    n += it->second*it->second;
                return n;
        }
        
        SparseVector_iterator begin(){
            return mData.begin();
        }
        
        SparseVector_iterator end(){
            return mData.end();
        }
        
};
                
}

typedef boost::shared_ptr<retin::SparseVector> SparseVectorRef;

#endif	/* __SPARSEVECTOR_H__ */
