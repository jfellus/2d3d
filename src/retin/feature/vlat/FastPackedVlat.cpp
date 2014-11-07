/*
Copyright © CNRS 2012. 
Authors: Philippe-Henri Gosselin, David Picard, Romain Négrel
Contact: philippe-henri.gosselin@ensea.fr

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
#include "FastPackedVlat.h"

#include "retin/toolbox/algebra/matrix_float.h"

#include <vector>

using namespace std;

namespace vlat {

FastPackedVlat::FastPackedVlat() : bagNorms(NULL),bagShifts(NULL),fNorm(0)
{
    
}   

FastPackedVlat::FastPackedVlat(float* data,size_t featureDim, size_t featureCount, size_t bagCount) :
    PackedVlat(data,featureDim,featureCount,bagCount),fNorm(0)
{
    bagNorms = new float[2*bagCount];
    bagShifts = bagNorms+bagCount;
}


FastPackedVlat::FastPackedVlat(size_t featureDim, size_t featureCount, size_t bagCount) :
    PackedVlat(featureDim,featureCount,bagCount),fNorm(0)
{
    bagNorms = new float[2*bagCount];
    bagShifts = bagNorms+bagCount;
}

FastPackedVlat::~FastPackedVlat() {
    if (bagNorms)
        delete[] bagNorms;
}

void    FastPackedVlat::init(const float* tensors,float power)
{
    if (power == 1) {
        if (!bagNorms) {
            bagNorms = new float[2*bagCount];
            bagShifts = bagNorms+bagCount;
        }
        fNorm = 0;
        size_t count = 0;
        for (size_t c = 0; c <bagCount; c++) {
            size_t size = bagSizes[c];
            if (size == 0) {
                bagShifts[c] = 0;
                bagNorms[c] = 0;
                continue;
            }
            vector<float> tmpf(featureDim * featureDim);
            const float* tensor = tensors + c * featureDim*featureDim;
            matrix_CpAAt_float(&tmpf[0], featureData + count * featureDim, featureDim, size);
            double px = matrix_ps_float(&tmpf[0], &tmpf[0], featureDim, featureDim);
            double ps = matrix_ps_float(&tmpf[0], tensor, featureDim, featureDim);
            double pn = matrix_ps_float(tensor, tensor, featureDim, featureDim);
            double n = bagInitialSizes[c];
            fNorm += px - 2 * n * ps + n * n*pn;
            bagShifts[c] = ps;
            bagNorms[c] = n * sqrt(pn);
            count += size;
        }
        fNorm = sqrt(fNorm);
    }
    else {
        fNorm = compNorm2(tensors,power);
    }
}

double  FastPackedVlat::dotprod (const FastPackedVlat& a,const FastPackedVlat& b)
{
    if (a.getBagCount() != b.getBagCount())
        retinThrowException2 ("Different bag count %d != %d",a.getBagCount(),b.getBagCount());
    size_t bagCount = a.getBagCount();
    if (a.getFeatureDim() != b.getFeatureDim())
        retinThrowException2 ("Different dim %d != %d",a.getFeatureDim(),b.getFeatureDim());
    size_t dim = a.getFeatureDim();

    size_t af = 0;
    size_t bf = 0;
    double sum = 0;
    for (size_t i=0;i<bagCount;i++) {
        size_t as = a.bagSizes[i];
        size_t bs = b.bagSizes[i];
        sum += matrix_ps_XXt_YYt_float (a.feature(af),as,b.feature(bf),bs,dim);
        sum -= a.bagShifts[i]*b.bagInitialSizes[i];
        sum -= b.bagShifts[i]*a.bagInitialSizes[i];
        sum += a.bagNorms[i]*b.bagNorms[i];
        af += as;
        bf += bs;
    }
    return sum / (a.fNorm*b.fNorm);

}

void  FastPackedVlat::dotprods (float* result,const FastPackedVlat& a,const FastPackedVlat* bs,size_t j0,size_t n,float power,const float* tensors)
{
    if (power == 1) {
        for (size_t j=0;j<n;j++) {
            const FastPackedVlat& b(bs[j0+j]);
            result[j] = dotprod(a,b);
        }
    }
    else 
    {
        size_t dim = a.getFeatureDim();
        size_t dim2 = dim*dim;
        size_t bagCount = a.getBagCount();

        vector<float> va(dim*dim*bagCount);
        size_t featureCount = 0;
        for (size_t c=0;c<bagCount;c++) {
            size_t size = a.getBagSize(c);
            if (size > 0) {
                matrix_CpAAt_float (&va[c*dim2],a.data()+featureCount*dim,dim,size);
                vector_addm_float(&va[c*dim2],-a.getBagInitialSize(c),tensors+c*dim2,dim2);
                vector_pow_float (&va[c*dim2],power,dim2);
            }
            featureCount += size;
        }

        vector<float> vb(dim*dim);
        for (size_t j=0;j<n;j++) {
            const FastPackedVlat& b(bs[j0+j]);
            size_t featureCount = 0;
            double res = 0;
            for (size_t c=0;c<bagCount;c++) {
                size_t size = b.getBagSize(c);
                if (size > 0 && a.getBagSize(c) > 0) {
                    vector_cpym_float(&vb[0],-b.getBagInitialSize(c),tensors+c*dim2,dim2);
                    matrix_CpAAt_float (&vb[0],b.data()+featureCount*dim,dim,size);
                    vector_pow_float (&vb[0],power,dim2);
                    res += vector_ps_float(&va[c*dim2],&vb[0],dim2);
                }
                featureCount += size;
            }
            res /= sqrt(b.fNorm*a.fNorm);
            result[j] = res;
        }
    }
}

}

