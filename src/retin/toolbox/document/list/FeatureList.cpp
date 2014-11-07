/**
 * \file FeatureList.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "FeatureList.h"
#include "BytesList.h"

#include "../serialization/Serializers.h"
#include "retin/toolbox/algebra/vector_float.h"

#include <vector>
#include <algorithm>

RETIN_SERIALIZABLE_OBJECT_DEFINITIONS(FeatureList);

namespace retin {

FeatureList::FeatureList (const BytesList& list) :
    nDim(list.dim()),nSize(list.size())
{
	size_t n = nDim;
	n *= nSize;
	if (n > 0) {
		pData.resize(n);
		const uchar* data = list.data();
		for (size_t i=0;i<n;i++)
			pData[i] = data[i];
	}
}

void	FeatureList::resize(int p,int n,bool bKeepData) 
{
	if (nSize == n && nDim == p)
		return;
        
        if(bKeepData && (size_t)nSize*(size_t)nDim == (size_t)n*(size_t)p){
                nSize = n;
                nDim = p;
                return;
        }
        
	if (!bKeepData || nSize == 0 || nDim == 0) {
		nSize = n;
		nDim = p;
		size_t m = n;
		pData.resize (m*p); 
		return;
	}
	if (p != nDim) {
		retinThrowException("FeatureList resizing unsupported if dim is changed");
	}
	else {
		jfloat* oldData = pData.release();
		pData.resize (size_t(n)*size_t(p));
		if (oldData) {
			memcpy(pData.get(),oldData,size_t(p)*size_t(std::min(n,nSize))*sizeof(jfloat));
			delete[] oldData;
		}
		nSize = n;
	}
}

void    FeatureList::powerNorm(float power, std::string norm){
    for(size_t i = 0 ; i < (size_t)nSize ; i++){
        if(power != 1.0f){
            vector_pow_float (&pData[i*nDim], power, nDim);
        }
        if(norm == "l2"){
            float n = vector_n2_float (&pData[i*nDim], nDim);
            if(n != 0.0f)
                vector_sdiv_float (&pData[i*nDim], n, nDim);
        }
    }
    
}

void	FeatureList::show (int i0,int n) const
{
	for (size_t i=0;i<(size_t)n;i++) {
		for (size_t k=0;k<(size_t)nDim;k++)
			printf ("% 6.2f ",pData[k+(i0+i)*nDim]);
		printf ("\n");
	}
}

void    FeatureList::addFeaturesList(FeatureList & featuresList, float perFeatures, int maxFeatures) {
                
                std::vector<int> index;                
                std::vector<int>::iterator it;

                for(int i = 0 ; i < featuresList.size() ; i++) index.push_back(i);
                random_shuffle (index.begin(), index.end());
                
                int nbFeature;
                
                if(perFeatures>=1.0)
                    nbFeature = featuresList.size();
                else
                    nbFeature = (int)((float)featuresList.size() * perFeatures);
                
                if(maxFeatures != 0 && nbFeature > maxFeatures)
                    nbFeature = maxFeatures;
            
                resize(nDim,nSize+nbFeature,true);
                it = index.begin();
                for(int i = nSize-nbFeature; i < nSize; i++, ++it)
                        memcpy(pData.get(size_t(i)*nDim), featuresList.data(*it), nDim*sizeof(float));
        }

void   FeatureList::subRandomFeatureList( float perFeatures, int maxFeatures) {
                std::vector<int> index;                
                std::vector<int>::iterator it;

                for(int i = 0 ; i < nSize ; i++) index.push_back(i);
                random_shuffle (index.begin(), index.end());

                int nbFeature = (int)((float)nSize * perFeatures);
                if(maxFeatures != 0 && nbFeature > maxFeatures)
                            nbFeature = maxFeatures;

                index.resize(nbFeature);

                sort (index.begin(), index.end());

                for(int i = 0; i < nbFeature; i++)
                        memcpy(pData.get(size_t(i)*nDim), pData.get(index[i]*nDim), nDim*sizeof(float));

                resize(nDim, nbFeature, true);
        }
}
