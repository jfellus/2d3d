/* 
 * File:   SparseMatrix.h
 * Author: romanegr
 *
 * Created on 8 juillet 2013, 18:24
 */

#ifndef __SPARSEMATRIX_H__
#define	__SPARSEMATRIX_H__
        
#include "Document.h"

#include "retin/toolbox/document/basic/SparseVector.h"
#include "retin/toolbox/document/basic/Feature.h"
#include "retin/toolbox/document/FloatMatrix.h"
#include "retin/toolbox/core/thread_pool.h"

#include <vector>

namespace retin
{

class	SparseMatrix : public Document
{
	RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin/toolbox/document,SparseMatrix);

	jint				nVectorSize,nVectorCount;
	std::vector<SparseVectorRef>	mData;

	template<class SerializerType>
	friend void	serialize(SerializerType& s, SparseMatrix& x)
	{
		s.object ("vectorSize",x.nVectorSize);
		s.object ("vectorCount",x.nVectorCount);
		s.object ("sVector",x.mData);
	}
        
        
public:
        SparseMatrix (const jint nVectorSize=1, const jint nVectorCount=1) :
                nVectorSize(nVectorSize),nVectorCount(nVectorCount){
                mData.clear();
                for(jint i = 0 ; i < nVectorCount ; i++){
                    mData.push_back( boost::make_shared<SparseVector>(nVectorSize));
                }
        }
        
        SparseMatrix (const jfloat * pData, const jint nVectorSize=1, const jint nVectorCount=1, const jfloat threshold = 0.0f) :
                nVectorSize(nVectorSize),nVectorCount(nVectorCount) {
                mData.clear();
                for(jint i = 0 ; i < nVectorCount ; i++){
                    mData.push_back( boost::make_shared<SparseVector>(pData+i*nVectorSize, nVectorSize, threshold));
                }
        }
        
        SparseMatrix (const SparseMatrix& o) :
                nVectorSize(o.nVectorSize),nVectorCount(o.nVectorCount) {
                mData.clear();
                for(jint i = 0 ; i < nVectorCount ; i++){
                    mData.push_back( boost::make_shared<SparseVector>(*(o.mData.at(i))));
                }
        }
        
        boost::shared_ptr<SparseMatrix> getSubMatrix(const jint nVectorSize=1, const jint nVectorCount=1){
            boost::shared_ptr<SparseMatrix> temp;
            if(this->nVectorSize < nVectorSize || this->nVectorCount < nVectorCount)
                return temp;
            temp = boost::make_shared<retin::SparseMatrix>(nVectorSize, nVectorCount);
            for(size_t i = 0 ; i < size_t(nVectorCount) ; i++){
                SparseVector_iterator it;
                for(it = mData[i]->begin(); it != mData[i]->end() ;it++){
                    if(it->first < nVectorSize){
                        temp->setValue(it->first, i, it->second);
                    }
                }
            }
            return temp;
        }
        
        jint		vectorSize() const { return nVectorSize; }
        
	jint		vectorCount() const { return nVectorCount; }
        
        jint		nbNonZeroVal() const { 
            jint nbval = 0;
            for(jint i = 0 ; i < nVectorCount ; i++){
                nbval += mData[i]->nbNonZeroVal();
            }
            return nbval;
        }
        
        jfloat          sparsityRate() const {
                return ((jfloat)nbNonZeroVal())/(((jfloat)vectorSize())*((jfloat)vectorCount()));
        }
        
        void		resize(const jint nVectorSize=1, const jint nVectorCount=1){
                BOOST_ASSERT (nVectorSize > 0);
                BOOST_ASSERT (nVectorCount > 0);
                if(nVectorCount != this->nVectorCount){
                    mData.resize(nVectorCount);
                    for(jint i = this->nVectorCount; i < nVectorCount ; i++){
                        mData.at(i) = boost::make_shared<SparseVector>(this->nVectorSize);
                    }
                    this->nVectorCount = nVectorCount;
                }
                if(nVectorSize != this->nVectorSize){
                    for(jint i = 0; i < this->nVectorCount ; i++){
                        mData.at(i)->resize(this->nVectorSize);
                    }
                    this->nVectorSize = nVectorSize;
                }
        }
        
        void            setValue(jint i, jint j,jfloat x) {
 		BOOST_ASSERT (i >= 0 && i < nVectorSize);
		BOOST_ASSERT (j >= 0 && j < nVectorCount);
		
                mData.at(j)->setValue(i, x);
        }
        
        jfloat          getValue(jint i, jint j) {
 		BOOST_ASSERT (i >= 0 && i < nVectorSize);
		BOOST_ASSERT (j >= 0 && j < nVectorCount);
                
                return mData.at(j)->getValue(i);
        }
        
        SparseVectorRef getVector(jint j) {
            BOOST_ASSERT (j >= 0 && j < nVectorCount);
            return mData.at(j);
        }
        
        void         compProjVect(jfloat * vectOut, const jfloat * vectIn, const jint n){
                BOOST_ASSERT (n == nVectorSize);
                for(jint j = 0 ; j < nVectorCount ; j++){
                    vectOut[j] = mData.at(j)->compDotProd(vectIn, n);
                }
        }
        
private:
    class compProjMatrix_conqueror : public thread_pool::conqueror {
        protected:
            std::vector<SparseVectorRef>&_mData;
            jfloat * _matrixOut;
            const jfloat * _matrixIn;
            const jint _n;
            const jint _m;
            const jint _nVectorCount;
            const jint _nVectorSize;
        public:
            compProjMatrix_conqueror(std::vector<SparseVectorRef>&mData, jfloat * matrixOut, const jfloat * matrixIn, const jint n, const jint m,const jint nVectorCount, const jint nVectorSize):
                _mData(mData),_matrixOut(matrixOut), _matrixIn(matrixIn), _n(n), _m(m), _nVectorCount(nVectorCount), _nVectorSize(nVectorSize){
            }
            void    conquer (size_t i0,size_t n) {
                if(_m>_nVectorCount){
                    for(jint i = i0 ; size_t(i) < i0+n ; i++){
                        for(jint j = 0 ; j < _nVectorCount ; j++){
                            _matrixOut[size_t(j)+size_t(_nVectorCount)*size_t(i)] = _mData.at(j)->compDotProd(_matrixIn+size_t(_nVectorSize)*size_t(i), _n);
                        }
                    }
                }
                else{
                    for(jint i = 0 ; i < _m ; i++){
                        for(jint j = i0 ; size_t(j) < i0+n ; j++){
                            _matrixOut[size_t(j)+size_t(_nVectorCount)*size_t(i)] = _mData.at(j)->compDotProd(_matrixIn+size_t(_nVectorSize)*size_t(i), _n);
                        }
                    }
                }
            }
        };
public:
        
        void         compProjMatrix(jfloat * matrixOut, const jfloat * matrixIn, const jint n, const jint m, thread_pool *pool =NULL){
                BOOST_ASSERT (n == nVectorSize);
                BOOST_ASSERT (pool != NULL);
                compProjMatrix_conqueror conqueror(mData, matrixOut, matrixIn, n, m, nVectorCount, nVectorSize);
                if(m>nVectorCount)
                        conqueror.divide_and_conquer(pool,0,m);
                else
                        conqueror.divide_and_conquer(pool,0,nVectorCount);
        }
        
        void         compOrtoProjVect(jfloat * vectOut, const jfloat * vectIn, const jint n){
                BOOST_ASSERT (n == nVectorSize);
                std::vector<float> temp(vectIn,vectIn+n);
                for(jint j = 0 ; j < nVectorCount ; j++){
                    vectOut[j] = mData.at(j)->compDotProd(&temp[0], n);
                    mData.at(j)->linear(&temp[0], &temp[0], -vectOut[j], n);
                }
        }
        
        FeatureRef  compProjVect(FeatureRef f){
                FeatureRef o = boost::make_shared<Feature>(nVectorCount);
                compProjVect(o->data(), f->data(), f->dim());
                return o;
        }
        
        FeatureRef         compOrtoProjVect(FeatureRef f){
                FeatureRef o = boost::make_shared<Feature>(nVectorCount);
                compOrtoProjVect(o->data(), f->data(), f->dim());
                return o;
        }
        
private:
    class add_mul_matrix_conqueror : public thread_pool::conqueror {
        protected:
            std::vector<SparseVectorRef>&_mData;
            FloatMatrixRef _m;
            jfloat _mul;
            jfloat _pNonZeroVal;
            bool _softThreshol;
        public:
            add_mul_matrix_conqueror(std::vector<SparseVectorRef>&mData,FloatMatrixRef m, jfloat mul, jfloat pNonZeroVal, bool softThreshol = false):
                _mData(mData),_m(m), _mul(mul), _pNonZeroVal(pNonZeroVal), _softThreshol(softThreshol){
                
            }
            void    conquer (size_t i0,size_t n) {
                for(jint i = i0 ; size_t(i) < i0+n; i++){
                    _mData[i]->add_mul_vector(_m->data(i), _mul, _pNonZeroVal, _m->vectorSize(), _softThreshol);
                }
            }
        };
public:
        void         add_mul_matrix(FloatMatrixRef m, jfloat mul, jfloat pNonZeroVal, bool softThreshol = false, thread_pool *pool =NULL){
            BOOST_ASSERT (m->vectorSize() != nVectorSize);
            BOOST_ASSERT (m->vectorCount() != nVectorCount);
            BOOST_ASSERT (pool != nVectorCount);
            add_mul_matrix_conqueror conqueror(mData, m, mul, pNonZeroVal, softThreshol);
            conqueror.divide_and_conquer(pool, 0, m->vectorCount());
            /*for(jint i = 0 ; i < m->vectorCount(); i++){
                mData[i]->add_mul_vector(m->data(i), mul, pNonZeroVal, m->vectorSize(), softThreshol);
            }*/
        }
        
        jfloat       norml2(){
            return sqrt(norml2p2());
        }
        jfloat       norml2p2(){
            jfloat n = 0.0f;
            for(jint i = 0 ; i < nVectorCount; i++){
                n += mData[i]->norml2p2();
            }
            return n;
        }
        
};


typedef boost::shared_ptr<SparseMatrix> SparseMatrixRef;

}


#endif	/* __SPARSEMATRIX_H__ */

