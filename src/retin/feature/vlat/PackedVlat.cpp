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
#include "PackedVlat.h"

#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"

#include <vector>

using namespace std;

namespace vlat {

PackedVlat::PackedVlat() : bagCount(0)
{
}

PackedVlat::PackedVlat(float* data,size_t featureDim,size_t featureCount,size_t bagCount) :
    Bag(data,featureDim,featureCount),bagCount(bagCount)
{
    bagSizes = new size_type[bagCount];
    bagInitialSizes = new size_type[bagCount];
    memset(bagSizes,0,bagCount*sizeof(size_type));
    memset(bagInitialSizes,0,bagCount*sizeof(size_type));
}

PackedVlat::PackedVlat(size_t featureDim,size_t featureCount,size_t bagCount) :
    Bag(featureDim,featureCount),bagCount(bagCount)
{
    bagSizes = new size_type[bagCount];
    bagInitialSizes = new size_type[bagCount];
    memset(bagSizes,0,bagCount*sizeof(size_type));
    memset(bagInitialSizes,0,bagCount*sizeof(size_type));
}

PackedVlat::~PackedVlat()
{
    if (bagSizes)
        delete[] bagSizes;
    if (bagInitialSizes)
        delete[] bagInitialSizes;
}

size_t  PackedVlat::memoryUsage() const
{
    return Bag::memoryUsage()
         + bagCount*2*sizeof(size_type);
}

void    PackedVlat::decode2 (float* vlat,const float* tensors2) const
{
    size_t count = 0;
    size_t vlatSize = bagCount*featureDim*featureDim;
    memset (vlat,0,vlatSize*sizeof(float));
    for (size_t b=0;b<bagCount;b++)
    {
        size_t size = bagSizes[b];
        if (size == 0)
            continue;
        matrix_CpAAt_float (vlat+b*featureDim*featureDim,feature(count),featureDim,size);
        if (tensors2)
            vector_addm_float(vlat+b*featureDim*featureDim,-bagInitialSizes[b],tensors2+b*featureDim*featureDim,featureDim*featureDim);
        count += size;
    }
}

double     PackedVlat::compNorm2 (const float* tensors2,float power) const
{
    size_t count = 0;
    double norm = 0;
    for (size_t b=0;b<bagCount;b++)
    {
        size_t size = bagSizes[b];
        if (size == 0)
            continue;
        vector<float> tmp(featureDim*featureDim);
        matrix_CpAAt_float (&tmp[0],feature(count),featureDim,size);
        if (tensors2)
            vector_addm_float(&tmp[0],-bagInitialSizes[b],tensors2+b*featureDim*featureDim,featureDim*featureDim);
        vector_pow_float (&tmp[0],power,featureDim*featureDim);
        double n = matrix_n2_float(&tmp[0],featureDim,featureDim);
        norm += n*n;
        count += size;
    }
    return norm;
}

double  PackedVlat::dotprod (const PackedVlat& a,const PackedVlat& b,float power,const float* tensors)
{
    if (a.getBagCount() != b.getBagCount())
        retinThrowException2 ("Different bag count %d != %d",a.getBagCount(),b.getBagCount());
    size_t bagCount = a.getBagCount();
    if (a.getFeatureDim() != b.getFeatureDim())
        retinThrowException2 ("Different dim %d != %d",a.getFeatureDim(),b.getFeatureDim());
    size_t dim = a.getFeatureDim();

    vector<float> va(dim*dim*bagCount);
    vector<float> vb(dim*dim*bagCount);


    a.decode2(&va[0],tensors);
    vector_pow_float (&va[0],power,va.size());
    vector_sdiv_float (&va[0],matrix_n2_float(&va[0],va.size(),1),va.size());

    b.decode2(&vb[0],tensors);
    vector_pow_float (&vb[0],power,vb.size());
    vector_sdiv_float (&vb[0],matrix_n2_float(&vb[0],vb.size(),1),vb.size());

    return matrix_ps_float(&va[0],&vb[0],va.size(),1);
}

void  PackedVlat::write(std::ostream& out)
{
    Bag::write(out);

    int32_t i32;
    i32 = (int32_t) bagCount; out.write((const char*)&i32,sizeof(i32));
    out.write((const char*)bagSizes,bagCount*sizeof(size_type));
    out.write((const char*)bagInitialSizes,bagCount*sizeof(size_type));
}

void  PackedVlat::read(std::istream& in)
{
    Bag::read(in);
    
    int32_t i32;
    in.read((char*)&i32,sizeof(i32)); bagCount = i32;
    if (bagSizes) delete[] bagSizes;
    bagSizes = new size_type[bagCount];
    in.read((char*)bagSizes,bagCount*sizeof(size_type));
    if (bagInitialSizes) delete[] bagInitialSizes;
    bagInitialSizes = new size_type[bagCount];
    in.read((char*)bagInitialSizes,bagCount*sizeof(size_type));
    
}

}
