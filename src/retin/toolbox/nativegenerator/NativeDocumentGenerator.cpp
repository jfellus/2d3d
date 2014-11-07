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