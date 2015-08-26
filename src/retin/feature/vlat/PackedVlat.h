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
#ifndef __vlat__PackedVlat_h__
#define __vlat__PackedVlat_h__

#include "Bag.h"

namespace vlat {

class PackedVlat : public Bag {
protected:
    static const size_t maxBagSize = 0x0FFFF;
    typedef uint16_t size_type;
    size_t          bagCount;
    size_type*      bagSizes;
    size_type*      bagInitialSizes;
public:
    PackedVlat();
    //! Takes pointer ownership.
    PackedVlat(float* data,size_t featureDim,size_t featureCount,size_t bagCount);
    PackedVlat(size_t featureDim,size_t featureCount,size_t bagCount);
    ~PackedVlat();

    void                setBagSizes(int* tab) { for (size_t i=0;i<bagCount;i++) bagSizes[i] = (size_type)tab[i]; }
    void                setBagInitialSizes(int* tab) { for (size_t i=0;i<bagCount;i++) bagInitialSizes[i] = (size_type)tab[i]; }

    size_t              getBagCount() const { return bagCount; }
    virtual size_t      memoryUsage() const;

    size_t              getBagSize(size_t i) const { return bagSizes[i]; }
    void                setBagSize(size_t i,size_t size) {
        if (size > maxBagSize)
            retinThrowException2("Bag size is too large %d > %d",size,maxBagSize);
        bagSizes[i] = size;
    }
    size_t              getBagInitialSize(size_t i) const { return bagInitialSizes[i]; }
    void                setBagInitialSize(size_t i,size_t size) {
        if (size > maxBagSize)
            retinThrowException2("Bag size is too large %d > %d",size,maxBagSize);
        bagInitialSizes[i] = size;
    }
    
    size_t              getVlatSize() const { return featureDim*featureDim*bagCount; }
    void                decode2 (float* vlat,const float* tensors2) const;
    double              compNorm2 (const float* tensors2,float power) const;
    static double       dotprod (const PackedVlat& a,const PackedVlat& b,float power,const float* tensors);
    
    virtual void        write(std::ostream& out);
    virtual void        read(std::istream& in);
};

}

#endif
