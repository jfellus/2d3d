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
#include "Bag.h"

namespace vlat {
    
Bag::Bag() :
    featureDim(0),featureCount(0),featureData(0)
{
    
}

Bag::Bag(float* data,size_t featureDim,size_t featureCount) :
    featureDim(featureDim),featureCount(featureCount),featureData(data)
{

}

Bag::Bag(size_t featureDim,size_t featureCount) :
    featureDim(featureDim),featureCount(featureCount)
{
    featureData = new float[featureDim*featureCount];
    memset(featureData,0,featureDim*featureCount*sizeof(float));
}

Bag::~Bag()
{
    if (featureData)
        delete[] featureData;
}

float*  Bag::releaseData()
{
    float* data = featureData;
    featureData = NULL;
    return data;
}

void    Bag::write(std::ostream& out)
{
    int32_t i32;
    i32 = (int32_t) featureDim; out.write((const char*)&i32,sizeof(i32));
    i32 = (int32_t) featureCount; out.write((const char*)&i32,sizeof(i32));
    out.write((const char*)featureData,featureDim*featureCount*sizeof(float));    
}

void    Bag::read(std::istream& in)
{
    int32_t i32;
    in.read((char*)&i32,sizeof(i32)); featureDim = i32;
    in.read((char*)&i32,sizeof(i32)); featureCount = i32;
    if (featureData) delete[] featureData;
    featureData = new float[featureDim*featureCount];
    in.read((char*)featureData,featureDim*featureCount*sizeof(float));
}

}
