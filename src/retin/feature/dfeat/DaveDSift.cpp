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
#include "DaveDSift.h"
#include "DaveGradient.h"

using namespace std;
using namespace retin;

namespace dfeat {

DaveDSift::DaveDSift() {

}
    
void    DaveDSift::clear() {
    AbstractDaveFeature::clear();
    if(magnitude) {
        magnitude.clear();
    }
}

void	DaveDSift::computeMagnitudeOrientation() {
	if(!input) {
		retinThrowException("no input !");
	}
	
	//gradient en X
	auto_array_ptr<float> dx(width*height);
	derivX(input, dx.get(), width, height);
	
	//gradient en Y
	auto_array_ptr<float> dy(width*height);
	derivY(input, dy.get(), width, height);
	
	//magnitude
	if(magnitude)
		magnitude.clear();
	magnitude.resize(width*height*8);
	gradientOrientationMagnitude(dx.get(), dy.get(), magnitude.get(), width, height);
	
}

//! Créé un masque gaussien correspondant à la tenêtre d'extraction du sift
void	DaveDSift::computeGaussianKernel(auto_array_ptr<float>& gaus, size_t scale) {
	
	float sigma = 2 * scale;
	
	//resize
	gaus.resize(16*scale*scale);
	
	for(int x = -2*scale ; x < (int)(2*scale) ; x++) {
		for(int y = -2*scale ; y < (int)(2*scale) ; y++) {			
			float z = (x*x)/sigma + (y*y)/sigma;
			gaus[x * 4*scale + y] = exp(-0.5*z);
		}
	}
	
}

void    DaveDSift::extract (size_t step,size_t scale) {

    if (!input)
        retinThrowException("no input");
      
    if (!magnitude) {
	    computeMagnitudeOrientation();
	}



	//reset descs
	descs.clear();
	
	//largeur - 2*1pix de bord - 1 sift
	size_t nb_w = (width  -2 - 4*scale)/ step + 1;
	size_t nb_h = (height -2 - 4*scale)/ step + 1;
	nb_descs = nb_w*nb_h;
	descs.resize(nb_descs*128);
	
	//gaussian kernel
	auto_array_ptr<float> gausskernel;
	computeGaussianKernel(gausskernel, scale);
	
	//big loop
	size_t rwidth = width - 1 - scale*4;
	size_t rheight = height - 1 - scale*4;
	size_t d = 0;
	
	float* ori[8];
	for(size_t o = 0 ; o < 8 ; o++)
		ori[o] = magnitude.get(o*width*height);
	auto_array_ptr<float> buffer(scale);
	
	for(size_t i = 1 ; i < rheight ; i+=step) {
		size_t di = i * width;
		for(size_t j = 1 ; j < rwidth ; j+=step) {
			size_t dj = di + j;
			float* des = descs.get(128*d);
			//std::cout << "desc at " << i << "," << j << " ";
			//scalex
			
			for(size_t o = 0 ; o < 8 ; o++) {			
				for(size_t sx = 0 ; sx < 4 ; sx++) {
					size_t dsx = dj + sx*scale*width;
					//std::cout << "X";
					for(size_t px = 0 ; px < scale ; px++) {
						size_t dp = dsx + px*width;
						//std::cout << ".";
						for(size_t sy = 0 ; sy < 4 ; sy++) {
							vector_linear_float(buffer.get(), ori[o] + dp + sy*scale,
														1.f,
														gausskernel.get(sx*4*scale+sy*scale), 
														scale);
							float m = vector_n1_float(buffer.get(), scale);
							size_t ind = sx*32+sy*8;
							des[ind+o] += m;
							
						}
					}
				}
			}
			
			//norm l2
			float ndes = vector_n2_float(des, 128);
			if(ndes > 1e-5) {
				vector_sdiv_float(des, ndes, 128);
			
				//clamp
				for(size_t x = 0 ; x < 128 ; x++)
					if(des[x] > 0.2f) des[x] = 0.2f;
				
				//norm l2
				ndes = vector_n2_float(des, 128);
				vector_sdiv_float(des, ndes, 128);
				d++;
			}
				
		}
	}
	nb_descs = d;
	
}

}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "DaveFeature.h"

namespace dfeat {

FeatureListRef      extractDaveDSift (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge, int size, const char* parameters)
{
    DaveDSift extractor;
    extractor.setFileInput(fileName,size);
    extractor.cropInput(minX,minY,maxX,maxY,marge);

    FeatureListRef  output = boost::make_shared<FeatureList>();
    size_t total = 0;
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
    return output;
}

}


#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/Parameters.h"
#include "retin/toolbox/document/ImageFile.h"

JNIEXPORT jobject JNICALL Java_retin_feature_dfeat_DaveFeature_extractDSift
  (JNIEnv * env, jobject thisObject, jstring imageFileObj, 
										jobject featureParamsObj)
{
    RETIN_JAVA_METHOD_BEGIN

    JavaEnv je(env);
    
    ParametersRef featureParams = je.createDocument<Parameters> (featureParamsObj);
    string imageFile = je.createString(imageFileObj);
    
    FeatureListRef features;

    size_t step = featureParams->getAs<size_t>("spacing");
    std::vector<size_t> scales;
        
    std::vector<std::string> stringScales = (*featureParams)("scales").getNames();
        
    for(unsigned int i = 0 ; i < stringScales.size() ; i++){
		scales.push_back(boost::lexical_cast<size_t> (stringScales[i]));
    }
            
	features = dfeat::extractDaveDSift (step, scales, 0, 0, 1, 1, imageFile.c_str());
	
    return je.createObject (features);

    RETIN_JAVA_METHOD_END
    return NULL;

}

#endif
