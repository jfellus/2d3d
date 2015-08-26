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

#include "NativeDepthImagesFeatureGenerator.h"
#include "DepthCamera.h"
#include "retin/feature/dfeat/DaveHog.h"
#include "retin/feature/color/ImageEqualization.h"
#include "retin/toolbox/document/MeshFile.h"
#include "retin/toolbox/codec3d/Codec3DOFF.h"

#include <GL/osmesa.h>
#include <GL/glu.h>
#include <iostream>

using namespace retin;
using namespace dfeat;

void NativeDepthImagesFeatureGenerator::initParams(ParametersRef params){
    std::cout << "set params called !" << std::endl;
			
    size_t proj = params->getAs<size_t>("nb_projections");
    size_t widt = params->getAs<size_t>("image_width");
    size_t heig = params->getAs<size_t>("image_height");


    setNbCams(proj);
    setWidth(widt);
    setHeight(heig);

    step = params->getAs<int>("spacing");

    scales.clear();
    vector<string> s = (*params)("scales").getNames();
    for(size_t i = 0 ; i < s.size() ; i++) {
            size_t sc;
            istringstream(s[i]) >> sc;
            scales.push_back(sc);
    }
}


FeatureListRef NativeDepthImagesFeatureGenerator::computeFeatureListFile(std::string addressFile){
    
    //cout << "file : " << addressFile << endl;

    Decoder3DOFF dec;
    Mesh mesh;
    dec.read_open(addressFile);
    dec.read_header();
    dec.read_data(&mesh);

    //cout << "mesh ok" << endl;

    // extract dhog
    FeatureListRef  output = boost::make_shared<FeatureList>();
    size_t total = 0;
    
    if(nb_cams == 0) 
		nb_cams = 1;
	for(size_t n = 0 ; n < nb_cams ; n++) {
		// random cam
		/*
		float x = rand() /(float) RAND_MAX * 2.0f - 1;
		float y = rand() /(float) RAND_MAX * 2.0f - 1;
		float z = rand() /(float) RAND_MAX * 2.0f - 1;
		*/
		Vector3d cam(DEPTH_IMAGE_CAMERAS[n%200][0], 
					 DEPTH_IMAGE_CAMERAS[n%200][1],
					 DEPTH_IMAGE_CAMERAS[n%200][2]);
		cam.normalize();

		//cout << "cam: " << cam.x << " " << cam.y << " " << cam.z << endl;

		auto_array_ptr<float> input;
		generateDepthImage(input, cam, mesh);
		
		DaveHog extractor;
		// equalize the resulting image
		equalize(input.get(),width*height,1);
		extractor.setInput(input.release(), width, height);
		//extractor.cropInput(minX,minY,maxX,maxY);


		for (size_t i=0;i<scales.size();i++) {
				extractor.extract(step,scales[i]);
				size_t descCount = extractor.getKeypointCount();
				size_t descSize = extractor.getDescriptorSize();
				const float* descs = extractor.getDescriptors();

				if (output->size() < (int)(total+descCount))
						output->resize(descSize,total+descCount*(scales.size()-i), true);
				for (size_t k=0;k<descCount;k++) {
						output->setFeature(total++,descs+k*descSize);
				}
		}
		output->resize (output->dim(),total, true);
    }

    return output;
}


size_t NativeDepthImagesFeatureGenerator::getDim(ParametersRef params){
    if(NativeFeatureListGenerator::getDim(params) == 0) {
        return 128;
    }
    return NativeFeatureListGenerator::getDim(params);
}


size_t NativeDepthImagesFeatureGenerator::getDim(){
    if(NativeFeatureListGenerator::getDim() == 0) {
        return 128;
    }
    return NativeFeatureListGenerator::getDim();
}


////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA
                
void NativeDepthImagesFeatureGenerator::initParentTable(ManagerRef manager, string baseName, string tableName) {
    NativeDocumentGenerator::initParentTable(manager, baseName, tableName);
    fileDir = table->getDataDir();
    if(!fileDir.empty() && fileDir[fileDir.length()-1] != '/')
        fileDir += "/";
    parentInitialized = true;
}

void NativeDepthImagesFeatureGenerator::initParentTable(TableRef _parentTable){
    NativeDocumentGenerator::initParentTable(_parentTable);
    fileDir = table->getDataDir();
    if(!fileDir.empty() && fileDir[fileDir.length()-1] != '/')
        fileDir += "/";
    parentInitialized = true;
}
/*
DocumentRef NativeDepthImagesFeatureGenerator::computeDocument(string key){
    if(parentInitialized == false)
        retinThrowException("NativeDaveFeatureGenerator : parent not initialized");
    
    // load 3D model
    MeshFileRef ifile = table->getDocumentAs<MeshFile>(key);
    string file = fileDir + ifile->get();
    
    return computeFile(file);
}
*/
std::string NativeDepthImagesFeatureGenerator::addressFileFromKey(std::string key){
    if(parentInitialized == false)
        retinThrowException("NativeDaveFeatureGenerator : parent not initialized");
    MeshFileRef ifile = table->getDocumentAs<MeshFile>(key);
    return fileDir + ifile->get();   
}



#include "retin/toolbox/document/basic/NativePointer.h"
#include "retin/toolbox/database/JavaManager.h"

JNIEXPORT void JNICALL Java_retin_feature_depthimages_DepthImagesGenerator_newIntance
  (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef output = boost::make_shared<NativePointer>(new DepthImages());

    je.setDocumentField(thisObj,"instance",output);
    
    RETIN_JAVA_METHOD_END
}

JNIEXPORT void JNICALL Java_retin_feature_depthimages_DepthImagesGenerator_deleteIntance
  (JNIEnv * env, jobject thisObj){
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    NativePointerRef instance = je.getDocumentField<NativePointer>(thisObj,"instance");
    if(instance == NULL)
        retinThrowException("NativeDocumentGenerator : instance not initialized");
    
    delete instance->getPointerAs<DepthImages>();
    
    RETIN_JAVA_METHOD_END
}

#endif

