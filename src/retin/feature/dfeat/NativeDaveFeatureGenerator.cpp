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
#include "NativeDaveFeatureGenerator.h"

#include "DaveFeature.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace retin;

size_t NativeDaveFeatureGenerator::getDim(ParametersRef params){
    if(NativeFeatureListGenerator::getDim(params) == 0) {
		if(params->has("type"))
			if(params->get("type")=="rgb")
				return 96;
        return 128;
    }
    return NativeFeatureListGenerator::getDim(params);
}
    
size_t NativeDaveFeatureGenerator::getDim(){
    if(NativeFeatureListGenerator::getDim() == 0) {
		if(featureType == "rgb")
			return 96;
        return 128;
    }
    return NativeFeatureListGenerator::getDim();
}

void NativeDaveFeatureGenerator::initParams(ParametersRef params){
    NativeFeatureListGenerator::initParams(params);
    if(params->has("spacing"))
        this->step = params->getAs<int>("spacing");
        
    if(params->has("minX") && params->has("minY") && params->has("maxX") && params->has("maxY")) {
        this->minX = params->getAs<float>("minX");
        this->minY = params->getAs<float>("minY");
        this->maxX = params->getAs<float>("maxX");
        this->maxY = params->getAs<float>("maxY");
    }
    
    size_t max = 0;
    if(params->has("scales")){
        scales.clear();
        vector<string> sScales = params->getSubParams("scales")->getNames();
        
        for(size_t i = 0 ; i < sScales.size() ; i++){
            size_t v = boost::lexical_cast<size_t> (sScales[i]);
            if(v > max)
                max = v;
            scales.push_back (v);
        }
        marge = (int) 2*max;
    }
    
    if(params->has("type"))
        featureType = params->get("type");
        
    if(params->has("maxSize"))
		maxSize = params->getAs<int>("maxSize");
    
    paramsInitialized = true;
}

FeatureListRef NativeDaveFeatureGenerator::computeFeatureListFile(string addressFile) {
    if(paramsInitialized == false)
        retinThrowException("NativeDaveFeatureGenerator : params not initialized");
        
    if(featureType == "sift")
        return dfeat::extractDaveDSift (step, scales, minX, minY, maxX, maxY, addressFile.c_str(), marge, maxSize);
    else if(featureType == "hog")
        return dfeat::extractDaveHog (step, scales, minX, minY, maxX, maxY, addressFile.c_str(), marge, maxSize);
    else if(featureType == "fastsift")
        return dfeat::extractDaveFastSift (step, scales, minX, minY, maxX, maxY, addressFile.c_str(), marge, maxSize);
    else if(featureType == "rgb")
        return dfeat::extractDaveRgb (step, scales, minX, minY, maxX, maxY, addressFile.c_str(), marge, maxSize);
    
    return dfeat::extractDaveHog (step, scales, minX, minY, maxX, maxY, addressFile.c_str(), marge, maxSize);
}

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/document/ImageFile.h"
#include "retin/toolbox/document/basic/NativePointer.h"
#include "retin/toolbox/nativegenerator/NativeFeatureListGenerator.h"

void NativeDaveFeatureGenerator::initParentTable(ManagerRef manager, string baseName, string tableName) {
    NativeDocumentGenerator::initParentTable(manager, baseName, tableName);
    fileDir = table->getDataDir();
    if(!fileDir.empty() && fileDir[fileDir.length()-1] != '/')
        fileDir += "/";
    parentInitialized = true;
}

void NativeDaveFeatureGenerator::initParentTable(TableRef _parentTable){
    NativeDocumentGenerator::initParentTable(_parentTable);
    fileDir = table->getDataDir();
    if(!fileDir.empty() && fileDir[fileDir.length()-1] != '/')
        fileDir += "/";
    parentInitialized = true;
}

std::string NativeDaveFeatureGenerator::addressFileFromKey(std::string key){
    if(parentInitialized == false)
        retinThrowException("NativeDaveFeatureGenerator : parent not initialized");
    ImageFileRef imagefile = table->getDocumentAs<ImageFile>(key);
    return fileDir + imagefile->get();   
}

JNIEXPORT void JNICALL Java_retin_feature_dfeat_NativeDaveFeatureGenerator_newIntance
        (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef output = make_shared<NativePointer>(new NativeDaveFeatureGenerator());

    je.setDocumentField(thisObj,"instance",output);
    
    RETIN_JAVA_METHOD_END
}

JNIEXPORT void JNICALL Java_retin_feature_dfeat_NativeDaveFeatureGenerator_deleteIntance
        (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj,"instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    delete instance->getPointerAs<NativeDaveFeatureGenerator>();
    
    RETIN_JAVA_METHOD_END
}

#endif
