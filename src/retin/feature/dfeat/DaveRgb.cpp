/*
Copyright © CNRS 2013. 
Authors: David Picard
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
#include "DaveRgb.h"
#include "retin/toolbox/algebra/algebra.h"
#include "retin/toolbox/imcodec/ImageCodec.h"
#include "retin/feature/color/ImageResample.h"

using namespace std;
using namespace retin;

namespace dfeat {


void    DaveRgb::clear() {
    AbstractDaveFeature::clear();
    if(integrales) {
        integrales.clear();
    }
}


void    DaveRgb::setFileInput (const char* fileName, int maxSize) {
    clear();
	loadImageRGB(image, width, height, fileName);
	if(!image) {
		retinThrowException1("%s: failed to read image",fileName);
	}
    input = new float[3*width*height];
    for (size_t i=0;i<3*width*height;i++) {
        input[i] = image[i];    
	}
	
	// resize to max size
	if(maxSize > 0) {
		float* temp;
		resample_image<float> (temp, input, height, width, 3, maxSize);
		delete [] input;
		input = temp; 
	}
}



void	DaveRgb::computeIntegrales() {
	
	integrales.resize(width*height*6);
	
	// recopie
	float* channels[6];
	for(size_t c = 0 ; c < 6 ; c++) {
		channels[c] = integrales.get(c*width*height);
	}
	for(size_t i = 0, j = 0 ; i < 3*width*height ; i+=3, j++) {
		channels[0][j] = input[i+0];
		channels[1][j] = input[i+0];
		channels[2][j] = input[i+1];
		channels[3][j] = input[i+1];
		channels[4][j] = input[i+2];
		channels[5][j] = input[i+2];
	}
	
	
	for(size_t ch = 0 ; ch < 6 ; ch+=2) {
		float *chan = integrales.get(ch*width*height);
		float *sqchan = integrales.get((ch+1)*width*height);
	
		// 1ere ligne
		memset(chan, 0, width*sizeof(float));
		memset(sqchan, 0, width*sizeof(float));
		
		for(size_t l = 1 ; l < height ; l++) {
			// 1ere col
			chan[l*width] = 0;
			sqchan[l*width] = 0;
			
			// autre col
			for(size_t c = 1 ; c < width ; c++) {
				// sum
				double A = chan[(l-1)*width+c-1];
				double B = chan[(l-1)*width+c];
				double C = chan[l*width+c-1];
				double D = chan[l*width+c];
				chan[l*width+c] = (float)(D+B+C-A);
				// sum of squares
				A = sqchan[(l-1)*width+c-1];
				B = sqchan[(l-1)*width+c];
				C = sqchan[l*width+c-1];
				D = sqchan[l*width+c]*sqchan[l*width+c];
				sqchan[l*width+c] = (float)(D+B+C-A);
			}
		}
	}	
}

void    DaveRgb::extract (size_t step,size_t scale) {
    
    
    
    if (!input)
        retinThrowException("no input");    
      
    if (!integrales) {        
	    computeIntegrales();
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
	descs.resize(nb_descs*96);
    
	//big loop
	size_t rwidth = width - 1 - scale*4;
	size_t rheight = height - 1 - scale*4;
	size_t d = 0;
	float* chan[6];
	for(size_t c = 0 ; c < 6 ; c++) {
		chan[c] = integrales.get(c*width*height);
	}
	
	for(size_t i = 1 ; i < rheight ; i+=step) {
		size_t di = i * width;
		for(size_t j = 1 ; j < rwidth ; j+=step) {
			size_t dj = di + j;
			float* des = descs.get(96*d);
			// image intégrale
			for(size_t c = 0 ; c < 6 ; c+=2) {			
				for(size_t sx = 0 ; sx < 4 ; sx++) {
					size_t dsx = dj + sx*scale*width;
					for(size_t sy = 0 ; sy < 4 ; sy++) {
						dsx = dj + sx*scale*width + sy*scale;
						// mean
						double A = *(chan[c] + dsx - width - 1);
						double B = *(chan[c] + dsx + scale - width - 1);
						double C = *(chan[c] + dsx + scale*width - width - 1);
						double D = *(chan[c] + dsx + scale*width + scale - width - 1);
						float mu = (float)(A+D-B-C) / float(scale*scale);
						des[sx*24+sy*6+c] = mu;
						// std
						A = *(chan[c+1] + dsx - width - 1);
						B = *(chan[c+1] + dsx + scale - width - 1);
						C = *(chan[c+1] + dsx + scale*width - width - 1);
						D = *(chan[c+1] + dsx + scale*width + scale - width - 1);
						des[sx*24+sy*6+c+1] = (float)(A+D-B-C)/float(scale*scale) - mu*mu;
					}
				}
			}
			
			//norm l2
			float ndes = vector_n2_float(des, 96);
			if( ndes > 1e-5 ) {
				vector_sdiv_float(des, ndes, 96);
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

FeatureListRef      extractDaveRgb (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge, int size, const char* parameters)
{
    DaveRgb extractor;
    size_t descSize;
   
    extractor.setFileInput(fileName, size);

    extractor.cropInput(minX,minY,maxX,maxY,marge);

    FeatureListRef  output = make_shared<FeatureList>();
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

JNIEXPORT jobject JNICALL Java_retin_feature_dfeat_DaveFeature_extractRgb
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
            
	features = dfeat::extractDaveRgb (step, scales, 0, 0, 1, 1, imageFile.c_str());
	
    return je.createObject (features);

    RETIN_JAVA_METHOD_END
    return NULL;

}

#endif
