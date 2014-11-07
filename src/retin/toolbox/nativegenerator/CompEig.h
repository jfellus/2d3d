/* 
 * File:   CompEig.h
 * Author: romanegr
 *
 * Created on 18 juin 2013, 11:09
 */

#ifndef __COMPEIG_H__
#define	__COMPEIG_H__

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_CompEig_compEigMatrices
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_CompEig_compCovMatrix
  (JNIEnv *, jobject, jobject, jobjectArray);

}

#endif

#endif	/* __COMPEIG_H__ */

