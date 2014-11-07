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
#ifndef __DAVEFEATURE_H__
#define __DAVEFEATURE_H__

#include "retin/toolbox/document/list/FeatureList.h"


using namespace retin;

namespace dfeat {

FeatureListRef      extractDaveHog (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge = 0, int size = 0 , const char* parameters="inputSpace=rgb,outputSpace=lum,equalize");


FeatureListRef      extractDaveFastSift (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge = 0, int size = 0 , const char* parameters="inputSpace=rgb,outputSpace=lum,equalize");


FeatureListRef      extractDaveDSift (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge = 0, int size = 0 , const char* parameters="inputSpace=rgb,outputSpace=lum,equalize");


FeatureListRef      extractDaveRgb (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge = 0, int size = 0 , const char* parameters="inputSpace=rgb,outputSpace=lum,equalize");

}


////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT jobject JNICALL Java_retin_feature_dfeat_DaveFeature_extractHog
  (JNIEnv *, jobject, jstring, jobject);
  
JNIEXPORT jobject JNICALL Java_retin_feature_dfeat_DaveFeature_extractFastSift
  (JNIEnv *, jobject, jstring, jobject);
  
JNIEXPORT jobject JNICALL Java_retin_feature_dfeat_DaveFeature_extractDSift
  (JNIEnv *, jobject, jstring, jobject);
  
JNIEXPORT jobject JNICALL Java_retin_feature_dfeat_DaveFeature_extractRgb
  (JNIEnv *, jobject, jstring, jobject);

}

#endif


#endif
