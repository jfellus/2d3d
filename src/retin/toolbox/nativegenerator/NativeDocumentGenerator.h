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
/**
 * \file NativeDocumentGenerator.h
 * \author Romain Negrel
 * \version 4.0
 */

#ifndef __retin_NativeDocumentGenerator_h__
#define __retin_NativeDocumentGenerator_h__

#include "retin/toolbox/core/core.h"
#include "retin/toolbox/document/basic/Integer.h"
#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/document/Parameters.h"

#include "vector"

#ifndef RETIN_DISABLE_JAVA
#include "retin/toolbox/database/Table.h"
#include "retin/toolbox/database/Manager.h"
#endif

namespace retin
{

class NativeDocumentGenerator {
protected:
    bool paramsInitialized;
    
#ifndef RETIN_DISABLE_JAVA
    bool parentInitialized;
    
    ManagerRef  manager;
    std::string baseURL;
    std::string tableName;
    TableRef    table;
#endif
    
public:
    NativeDocumentGenerator() {
#ifndef RETIN_DISABLE_JAVA
        parentInitialized = false;
#endif
        paramsInitialized = false;
    }
    
    virtual ~NativeDocumentGenerator() {}
    
#ifndef RETIN_DISABLE_JAVA

    virtual void initParentTable(ManagerRef _manager, std::string _baseName, std::string _tableName){
        this->manager = _manager;
        this->baseURL = _baseName;
        this->tableName = _tableName;
        this->table = manager->getTable(this->baseURL, this->tableName);
    }
    
    virtual void initParentTable(TableRef _parentTable){
        this->table = _parentTable;
    }
    
    void getListDocsParentTable(std::vector<std::string > &keys){
        this->table->getKeys(keys);
    }
    
    virtual std::string addressFileFromKey(std::string key){
        retinThrowException("NativeDocumentGenerator : addressFileFromKey() : No implemented");
        return "";
    }
    
    DocumentRef computeDocument(std::string key){
        return computeFile(addressFileFromKey(key));
    }
#endif
    
    virtual void initParams(ParametersRef params){
        retinThrowException("NativeDocumentGenerator : initParams(ParametersRef params) : No implemented");
    }
    
    virtual void reset(){
        retinThrowException("NativeDocumentGenerator : reset() : No implemented");
    }
    
    virtual DocumentRef computeFile(std::string addressFile){
        retinThrowException("NativeDocumentGenerator : computeFile(std::string addressFile) : No implemented");
        return boost::make_shared<retin::Integer>();
    }
    
    virtual size_t getDim(ParametersRef params){
        retinThrowException("NativeDocumentGenerator : getDim(ParametersRef params) : No implemented");
        return 0;
    }
    
    virtual size_t getDim(){
        retinThrowException("NativeDocumentGenerator : getDim() : No implemented");
        return 0;
    }
    
    virtual bool isInitialized(){
#ifndef RETIN_DISABLE_JAVA                
        return parentInitialized && paramsInitialized;
#else
        return paramsInitialized;
#endif
    }
};

typedef boost::shared_ptr<NativeDocumentGenerator> NativeDocumentGeneratorRef;

}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

/*JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_initParentTable
        (JNIEnv *, jobject, jobject, jstring, jstring);*/

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_initParentTable
        (JNIEnv *, jobject, jobject);

JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_initParams
        (JNIEnv *, jobject, jobject);
    
JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_reset
        (JNIEnv *, jobject);
    
JNIEXPORT jobject JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_computeDocument
        (JNIEnv *, jobject, jstring);
    
JNIEXPORT jboolean JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_isInitialized
        (JNIEnv *, jobject);

JNIEXPORT jint JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_getDim
        (JNIEnv *, jobject, jobject);

/*
JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_newIntance
  (JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_retin_toolbox_nativegenerator_NativeDocumentGenerator_deleteIntance
  (JNIEnv *, jobject);
*/

}

#endif //RETIN_DISABLE_JAVA

#endif //__retin_NativeDocumentGenerator_h__
