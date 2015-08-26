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
#include "NativeDocumentGenerator.h"

using namespace std;
using namespace boost;
using namespace retin;

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/database/JavaManager.h"
#include "retin/toolbox/database/Table.h"
#include "retin/toolbox/document/Parameters.h"

#include "retin/toolbox/document/basic/NativePointer.h"

/*JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_initParentTable
        (JNIEnv * env, jobject thisObj, jobject managerObj, jstring baseNameObj, jstring tableNameObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj, "instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    NativeDocumentGenerator * ndg = instance->getPointerAs<NativeDocumentGenerator>();
    
    string baseName = je.createString(baseNameObj);
    string tableName = je.createString(tableNameObj);
    
    ManagerRef manager = make_shared<JavaManager>(env, managerObj);
    
    ndg->initParentTable(manager, baseName, tableName);
    
    RETIN_JAVA_METHOD_END
}*/

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_initParentTable
        (JNIEnv * env, jobject thisObj, jobject tableObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj, "instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    NativeDocumentGenerator * ndg = instance->getPointerAs<NativeDocumentGenerator>();
    
    TableRef parentTable = JavaManager::GetTable( je, tableObj );
    
    ndg->initParentTable(parentTable);
    
    RETIN_JAVA_METHOD_END
}

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_initParams
        (JNIEnv * env, jobject thisObj, jobject paramsObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj, "instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    NativeDocumentGenerator * ndg = instance->getPointerAs<NativeDocumentGenerator>();
    
    
    ParametersRef params = je.createDocument<Parameters> (paramsObj);
    
    ndg->initParams(params);
    
    RETIN_JAVA_METHOD_END
}
    
JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_reset
        (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj, "instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    NativeDocumentGenerator * ndg = instance->getPointerAs<NativeDocumentGenerator>();
    
    ndg->reset();
    
    RETIN_JAVA_METHOD_END
}
        
JNIEXPORT jobject JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_computeDocument
        (JNIEnv * env, jobject thisObj, jstring keyObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj, "instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    NativeDocumentGenerator * ndg = instance->getPointerAs<NativeDocumentGenerator>();
    
    string key = je.createString(keyObj);
    
    return je.createObject (ndg->computeDocument(key));
    
    RETIN_JAVA_METHOD_END
    return NULL;
}
    
JNIEXPORT jboolean JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_isInitialized
        (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj, "instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    NativeDocumentGenerator * ndg = instance->getPointerAs<NativeDocumentGenerator>();
    
    return ndg->isInitialized();
    
    RETIN_JAVA_METHOD_END
    return false;
}

JNIEXPORT jint JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_getDim
        (JNIEnv * env, jobject thisObj, jobject paramsObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj, "instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    NativeDocumentGenerator * ndg = instance->getPointerAs<NativeDocumentGenerator>();
    
    
    ParametersRef params = je.createDocument<Parameters> (paramsObj);
    
    return ndg->getDim(params);
    RETIN_JAVA_METHOD_END
    return 0;
}

/*
JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_newIntance
        (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef output = make_shared<NativePointer>(new NativeDocumentGenerator());

    je.setDocumentField(thisObj,"instance",output);
    
    RETIN_JAVA_METHOD_END
}

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_deleteIntance
        (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj,"instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    delete instance->getPointerAs<NativeDocumentGenerator>();
    
    RETIN_JAVA_METHOD_END
}
*/


#endif //RETIN_DISABLE_JAVA
