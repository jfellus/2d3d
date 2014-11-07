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
#include "DaveHog.h"
#include "DaveGradient.h"

using namespace std;
using namespace retin;

namespace dfeat {


void    DaveHog::clear() {
    AbstractDaveFeature::clear();
    if(magnitude) {
        magnitude.clear();
    }
}

#define IMAGE_INT
void	DaveHog::computeMagnitudeOrientation(float *gx, float *gy) {
    
	if(!input) {
                std::cout<<"no input !"<<std::endl;
		retinThrowException("no input !");                
	}
        
    //gradient en X
	auto_array_ptr<float> dx(width*height);
    if(!gx) {
		derivX(input, dx.get(), width, height);
	}
	else {
		dx.copy(gx, width*height);
	}
  	//gradient en Y
	auto_array_ptr<float> dy(width*height);
	if(!gy) {
		derivY(input, dy.get(), width, height);
	}
	else {
		dy.copy(gy, width*height);
	}
	
	//magnitude
	if(magnitude)
		magnitude.clear();
        
	magnitude.resize(width*height*8);
                        
	gradientOrientationMagnitude(dx.get(), dy.get(), magnitude.get(), width, height);        
	
	// image intégrale
#ifdef IMAGE_INT

	for(size_t o = 0 ; o < 8 ; o++) {
		float *ori = magnitude.get(o*width*height);
	
		// 1ere ligne
		memset(ori, 0, width*sizeof(float));
		
		for(size_t l = 1 ; l < height ; l++) {
			// 1ere col
			ori[l*width] = 0;
			
			// autre col
			for(size_t c = 1 ; c < width ; c++) {
				double A = ori[(l-1)*width+c-1];
				double B = ori[(l-1)*width+c];
				double C = ori[l*width+c-1];
				double D = ori[l*width+c];
				ori[l*width+c] = (float)(D+B+C-A);
			}
		}
	}
	
#endif
}

void    DaveHog::extract (size_t step,size_t scale, float *gx, float *gy) {
    
    
    
    if (!input)
        retinThrowException("no input");    
      
    if (!magnitude || gx || gy) {        
	    computeMagnitudeOrientation(gx, gy);
	}
	//reset descs
	descs.clear();
	//largeur - 2*1pix de bord - 1 sift
	size_t nb_w = (width  -2 - 4*scale)/ step + 1;
	size_t nb_h = (height -2 - 4*scale)/ step + 1;
	if(height < 2 + 4*scale)
		nb_h = 0;
	if(width < 2 + 4*scale)
		nb_w = 0;
	nb_descs = nb_w*nb_h;
	if(nb_descs == 0)
		return;
	descs.resize(nb_descs*128);
    
	//big loop
	size_t rwidth = width - 1 - scale*4;
	size_t rheight = height - 1 - scale*4;
	size_t d = 0;
	float* ori[8];
	for(size_t o = 0 ; o < 8 ; o++)
		ori[o] = magnitude.get(o*width*height);
	
	for(size_t i = 1 ; i < rheight ; i+=step) {
		size_t di = i * width;
		for(size_t j = 1 ; j < rwidth ; j+=step) {
			size_t dj = di + j;
			float* des = descs.get(128*d);
			//std::cout << "desc at " << i << "," << j << " ";
			//scalex
#ifndef IMAGE_INT			
			for(size_t o = 0 ; o < 8 ; o++) {			
				for(size_t sx = 0 ; sx < 4 ; sx++) {
					size_t dsx = dj + sx*scale*width;
					//std::cout << "X";
					for(size_t px = 0 ; px < scale ; px++) {
						size_t dp = dsx + px*width;
						//std::cout << ".";
						for(size_t sy = 0 ; sy < 4 ; sy++) {
							float m = vector_n1_float(ori[o] + dp 
													+ sy*scale, scale);
							des[sx*32+sy*8+o] += m;
						}
					}
				}
			}
#endif
#ifdef IMAGE_INT
			// image intégrale
			for(size_t o = 0 ; o < 8 ; o++) {			
				for(size_t sx = 0 ; sx < 4 ; sx++) {
					size_t dsx = dj + sx*scale*width;
					for(size_t sy = 0 ; sy < 4 ; sy++) {
						dsx = dj + sx*scale*width + sy*scale;
						double a = *(ori[o] + dsx - width - 1);
						double b = *(ori[o] + dsx + scale - width - 1);
						double c = *(ori[o] + dsx + scale*width - width - 1);
						double d = *(ori[o] + dsx + scale*width + scale - width - 1);
						des[sx*32+sy*8+o] = (float)(a+d-b-c);
					}
				}
			}
#endif
		
			//norm l2
			float ndes = vector_n2_float(des, 128);
			if( ndes > 1e-5 ) {
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

using namespace boost;

namespace dfeat {

FeatureListRef      extractDaveHog (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge, int size, const char* parameters)
{
    DaveHog extractor;
    size_t descSize;
   
    extractor.setFileInput(fileName, size);

    extractor.cropInput(minX,minY,maxX,maxY,marge);
    FeatureListRef  output = make_shared<FeatureList>();
    
    if(extractor.getHeight() < 3 || extractor.getWidth() < 3){
        output->resize (extractor.getDescriptorSize(),1, true);
        return output;
    }

    size_t total = 0;
    for (size_t i=0;i<scales.size();i++) {
        extractor.extract(step,scales[i]);
        size_t descCount = extractor.getKeypointCount();
        descSize = extractor.getDescriptorSize();
        const float* descs = extractor.getDescriptors();

        if (output->size() < (int)(total+descCount))
            output->resize(descSize,total+descCount*(scales.size()-i), true);
        for (size_t k=0;k<descCount;k++) {
            output->setFeature(total++,descs+k*descSize);
        }
    }
    if(total == 0) {
        /*cout << "fileName = " << fileName << " minX = " << minX << " minY = " << minY << " maxX = " << maxX << " maxY = " << maxY << endl;
        for (size_t i=0;i<scales.size();i++) {
            cout << "widthMin = heightMin = " << 2+scales[i]*4 << endl;
        }
        cout << "width = " << extractor.getWidth() << endl;
        cout << "height = " << extractor.getHeight() << endl;*/
        total = 1;
    }
        
    output->resize (descSize,total, true);
    return output;
}

}

#include "retin/toolbox/document/serialization/Java.h"
#include "retin/toolbox/document/list/ArrayList.h"
#include "retin/toolbox/document/list/FeatureList.h"
#include "retin/toolbox/document/Parameters.h"
#include "retin/toolbox/document/ImageFile.h"

JNIEXPORT jobject JNICALL Java_retin_feature_dfeat_DaveFeature_extractHog
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
            
	features = dfeat::extractDaveHog (step, scales, 0, 0, 1, 1, imageFile.c_str());
	
    return je.createObject (features);

    RETIN_JAVA_METHOD_END
    return NULL;

}

#endif
