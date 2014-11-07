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
#include "AbstractDaveFeature.h"

#include "retin/toolbox/imcodec/ImageCodec.h"
#include "retin/feature/color/ImageProcessing.h"
#include "retin/feature/color/ImageResample.h"


using namespace std;
using namespace retin;

namespace dfeat {

AbstractDaveFeature::AbstractDaveFeature() :
    width(0),height(0),image(NULL),palette(NULL),input(NULL) {

}

AbstractDaveFeature::~AbstractDaveFeature() {
    clear();
}

void    AbstractDaveFeature::clear() {
    if (input) {
        delete [] input;
        input = NULL;
    }
    if (image) {
        delete [] image;
        image = NULL;
    }
    if (palette) {
        delete [] palette;
        palette = NULL;
    }
    width = 0;
    height = 0;
}


void    AbstractDaveFeature::setInput (float* input_,size_t width_,size_t height_) {
    if (input) {
        delete [] input;
        input = NULL;
    }
    width = width_;
    height = height_;
    //input = input_;    //modif olivier Kihl remplacé par  v    
    input = new float[width*height];
    for (size_t i=0;i<width*height;i++)
        input[i] = input_[i] ;
            
}

void    AbstractDaveFeature::setFileInput (const char* fileName, int maxSize) {
    clear();
    
	loadImageRGB(image, width, height, fileName);
	if(!image) {
			retinThrowException1("%s: failed to read image",fileName);
	}
    auto_array_ptr<unsigned char> temp(width*height);
  
	// convert to Lum
    rgb2lum (temp.get(),image,width*height);
	// equalize the resulting image
    equalize(temp.get(),width*height,1);
    
    // resize to max size
    if(maxSize > 0) {
		float *temp2 = new float[width*height];
		for(size_t i = 0 ; i < width*height ; i++) {
			temp2[i] = temp[i];
		}
		resample_image<float> (input, temp2, height, width, 1, maxSize);
		
		delete [] temp2;
	}
	else {	
		input = new float[width*height];
		for(size_t i = 0 ; i < width*height ; i++) {
			input[i] = temp[i];
		}
	}
}


void    AbstractDaveFeature::cropInput (float minX,float minY,float maxX,float maxY, int marge) {
    
    //cout << minX << " " << minY << " " << maxX << " " << maxY << " " << marge << endl;
    int iminX = minX * width - marge;
    int imaxX = maxX * width + marge;
    //cout << width << " " << height << endl;
    if(iminX < 0)
        iminX = 0;
    if(imaxX > (int)width)
        imaxX = width;
    
    int iminY = minY * height - marge;
    int imaxY = maxY * height + marge;
    
    if(iminY < 0)
        iminY = 0;
    if(imaxY > (int)height)
        imaxY = height;
    //cout << iminX << " " << iminY << " " << imaxX << " " << imaxY << endl << endl;
    cropInput(iminX,iminY,imaxX,imaxY);
}

void    AbstractDaveFeature::cropInput (int minX,int minY,int maxX,int maxY) {
    if (minX == 0 && minY == 0 && maxX == (int)width && maxY == (int)height)
        return;

    int oldwidth = width;
    float* oldinput = input;

    width = maxX - minX;
    height = maxY - minY;
    input = new float [width*height];


    for (size_t j=0;j<height;j++)
    for (size_t i=0;i<width;i++)
        input[ i + j*width] = oldinput[ minX+i + (minY+j)*oldwidth ];

    delete[] oldinput;
}

void      extractDaveFeature (float*& descriptors,size_t& descriptorCount,AbstractDaveFeature& extractor,        
        size_t step,const std::vector<size_t>& scales)
{
    descriptorCount = 0;
    size_t descSize = extractor.getDescriptorSize();
    size_t sizes[100];
    float* buffers[100];    
    for (size_t i=0;i<scales.size();i++) 
    {
        extractor.extract(step,scales[i]);
        size_t descCount = extractor.getKeypointCount();        
        const float* descs = extractor.getDescriptors();
    

        buffers[i] = new float[descCount*descSize];
        memcpy (buffers[i],descs,descCount*descSize*sizeof(float));
        sizes[i] = descCount * descSize;
        descriptorCount += descCount;

    }
    if (descriptors) delete[] descriptors;
    size_t j = 0;
    descriptors = new float[descriptorCount * descSize];
    for (size_t i=0;i<scales.size();i++) 
    {
        memcpy(descriptors+j,buffers[i],sizes[i]*sizeof(float));
        delete[] buffers[i];
        j += sizes[i];
    }
}


}

#ifndef RETIN_DISABLE_JAVA

#include "retin/toolbox/document/Parameters.h"
#include "retin/feature/color/ImageColor.h"

namespace dfeat {

void    AbstractDaveFeature::setFileInput (const char* fileName,const char* parameters) {
    uchar *image=NULL,*palette=NULL;
    size_t width,height,channels;
    loadImage(image,palette,width,height,channels,fileName);
    std::cout << "setFileInput called" << std::endl;

    ParametersRef params = boost::make_shared<Parameters>(parameters);
    BytesMatrixRef matrix = boost::make_shared<BytesMatrix>((jbyte*)image,width,height,channels,true);
    
    BytesMatrixRef temp;
    retin::convertColors<BytesMatrix> (temp, matrix, params);
    if (temp->dim() != 1)
        retinThrowException1("Invalid dim %d", temp->dim());
    float* buf = new float[ temp->width() * temp->height() ];
    size_t k = 0;
    for (size_t j = 0; j < (size_t) temp->height(); j++)
        for (size_t i = 0; i < (size_t) temp->width(); i++)
            buf[k++] = (*temp)(i, j);
    setInput(buf, temp->width(), temp->height());
    
}

}

#endif
