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
#ifndef __vlat__FastPackedVlat_h__
#define __vlat__FastPackedVlat_h__

#include "PackedVlat.h"

namespace vlat {

class FastPackedVlat : public PackedVlat {
protected:    
    float*          bagNorms;
    float*          bagShifts;
    float           fNorm;
public:
    FastPackedVlat();
    //! Takes pointer ownership.
    FastPackedVlat(float* data,size_t featureDim,size_t featureCount,size_t bagCount);
    FastPackedVlat(size_t featureDim,size_t featureCount,size_t bagCount);
    ~FastPackedVlat();

    virtual void    init(const float* tensors,float power=1);

    const float*    getBagNorms() const { return bagNorms; }
    const float*    getBagShifts() const { return bagShifts; }


    static double       dotprod (const FastPackedVlat& a,const FastPackedVlat& b);
    static void         dotprods (float* result,const FastPackedVlat& a,const FastPackedVlat* bs,size_t i0,size_t n,float power,const float* tensors);
    
};

}

#endif
