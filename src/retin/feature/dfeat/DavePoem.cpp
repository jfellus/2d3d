/*
Copyright © CNRS 2013. 
Authors: David Picard, Son Vu
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
#include "DavePoem.h"
#include "DaveGradient.h"

using namespace std;
using namespace retin;

namespace dfeat {


void    DavePoem::clear() {
    AbstractDaveFeature::clear();
    if(magnitude) {
        magnitude.clear();
    }
}

#define IMAGE_INT
void	DavePoem::computeMagnitudeOrientation(float *gx, float *gy) {
    
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
        
	magnitude.resize(width*height*nb_orientation);
    if(nb_orientation > 1)     
		gradientNbOrientationMagnitude(dx.get(), dy.get(), magnitude.get(), width, height, nb_orientation);
	else // LBP
		gradientMagnitude(dx.get(), dy.get(), magnitude.get(), width, height);
	
	//std::cout << "magnitude orientation " << nb_orientation << " ok" << std::endl;

}

void    DavePoem::extract (size_t step,size_t cell, float *gx, float *gy) {
    
    //this->cell = cell;
    
    if (!input)
        retinThrowException("no input");    
      
    if (!magnitude || gx || gy) {        
	    computeMagnitudeOrientation(gx, gy);
	}
	//reset descs
	descs.clear();
	//largeur - 2*1pix de bord - 1 sift
	size_t nb_w = (width  -2 - nb_cell*cell)/ step + 1;
	size_t nb_h = (height -2 - nb_cell*cell)/ step + 1;
	if(height < 2 + nb_cell*cell)
		nb_h = 0;
	if(width < 2 + nb_cell*cell)
		nb_w = 0;
	nb_descs = nb_w*nb_h;
	if(nb_descs == 0)
		return;
/*
	std::cout << "scale : " << scale << std::endl;
	std::cout << "radius : " << radius << std::endl;
	std::cout << "neighbors : " << neighbors << std::endl;
	std::cout << "orientation : " << nb_orientation << std::endl;
	std::cout << "cell : " << nb_cell << std::endl;
*/
	size_t nb_bins = 1<<neighbors;
	computeSize();
//	std::cout << "desc_size = " << desc_size << std::endl;
	descs.resize(nb_descs*desc_size);
    
    // preprocessing
    	// calcul image integrale
	for(size_t o = 0 ; o < nb_orientation ; o++) {
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
	//std::cout << "image int ok" << std::endl;
	// moyenne en utilisant l'image integrale
	auto_array_ptr<float> temp(width*height*nb_orientation);
		
	for(size_t o = 0 ; o < nb_orientation ; o++) {	
		float *ori = magnitude.get(o*width*height);			
		for(size_t i = scale/2+1 ; i < height-scale/2-1 ; i++) {
			size_t di = (i-scale/2) * width;
			for(size_t j = scale/2+1 ; j < width-scale/2-1 ; j++) {		
				size_t dsx = di+j-scale/2;
				double a = *(ori + dsx - width - 1);
				double b = *(ori + dsx + scale - width - 1);
				double c = *(ori + dsx + scale*width - width - 1);
				double d = *(ori + dsx + scale*width + scale - width - 1);
				temp[o*width*height+i*width+j] = (float)(a+d-b-c);
			}
		}
	}
	//std::cout << "filtrage moyenneur ok" << std::endl;
	
	// calcul du code LBP
	// 1) LUT des coordonées des voisins sur le cercle
	float* rx = new float[neighbors];
	float* ry = new float[neighbors];
	size_t* frx = new size_t[neighbors];
	size_t* fry = new size_t[neighbors];
	size_t* crx = new size_t[neighbors];
	size_t* cry = new size_t[neighbors];
	float* x = new float[neighbors];
	float* y = new float[neighbors];
	for(size_t ne = 0 ; ne < neighbors ; ne++) {
				rx[ne] = radius*cos(ne*2*M_PI/neighbors);
				ry[ne] = radius*sin(ne*2*M_PI/neighbors);
				frx[ne] = floorf(rx[ne]);
				fry[ne] = floorf(ry[ne]);
				crx[ne] = ceilf(rx[ne]);
				cry[ne] = ceilf(ry[ne]);
				x[ne] = rx[ne] - frx[ne];
				y[ne] = ry[ne] - fry[ne];
	}
	// 2) calcul du code en tout point
	auto_array_ptr<unsigned int> code(width*height*nb_orientation);
	memset(code.get(), 0, width*height*nb_orientation*sizeof(unsigned int));
	for(size_t i = radius ; i < height-radius ; i++) {
		for(size_t j = radius ; j < width - radius ; j++) {
			for(size_t ne = 0 ; ne < neighbors ; ne++) {
					
				for(size_t o = 0 ; o < nb_orientation ; o++) {
					float *ori = temp.get(o*width*height);
					float r = *(ori+i*width+j);
					float p1 = *(ori+(i+fry[ne])*width+j+frx[ne]) *(1-y[ne])*(1-x[ne]);
					float p2 = *(ori+(i+fry[ne])*width+j+crx[ne]) *(1-y[ne])*x[ne];
					float p3 = *(ori+(i+cry[ne])*width+j+frx[ne]) *y[ne]*(1-x[ne]);
					float p4 = *(ori+(i+cry[ne])*width+j+crx[ne]) *y[ne]*x[ne];
					
					float p = (p1+p2+p3+p4);
					// code LBP
					*(code.get(o*width*height)+i*width+j) += ( (r>=p) << ne);
				}
			}
		}
	}
	delete [] rx;
	delete [] ry;
	delete [] frx;
	delete [] fry;
	delete [] crx;
	delete [] cry;
	delete [] x;
	delete [] y;
	
	//std::cout << "code lpb ok" << std::endl;
    
	//big loop
	size_t rwidth = width - 1 - nb_cell*cell;
	size_t rheight = height - 1 - nb_cell*cell;
	size_t d = 0;
	
	for(size_t i = 1 ; i < rheight ; i+=step) {
		size_t di = i * width;
		for(size_t j = 1 ; j < rwidth ; j+=step) {
			size_t dj = di + j;
			float* des = descs.get(desc_size*d);
			//std::cout << "desc at " << i << "," << j << " " << std::endl;
			//scalex
					
			for(size_t o = 0 ; o < nb_orientation ; o++) {			
				for(size_t sx = 0 ; sx < nb_cell ; sx++) {
					size_t dsx = dj + sx*scale*width;
					//std::cout << "X";
					for(size_t px = 0 ; px < cell ; px++) {
						size_t dpx = dsx + px*width;
						//std::cout << ".";
						for(size_t sy = 0 ; sy < nb_cell ; sy++) {
							size_t dpy = dpx + sy*cell;
							for(size_t py = 0 ; py < cell ; py++) {
								size_t dp = dpy + py;
								unsigned int id = *(code.get(o*width*height)+dp);
								des[o*nb_bins*nb_cell*nb_cell + (sx*nb_cell+sy)*nb_bins + id] += 1;
							}
						}
					}
				}
			}
		
			//norm l2
			float ndes = vector_n2_float(des, desc_size);
			if( ndes > 1e-5 ) {
				vector_sdiv_float(des, ndes, desc_size);
				d++;
			}
				
		}
	}
	nb_descs = d;
	//std::cout << "nbdesc : " << nb_descs << std::endl;
}


}

////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include "DaveFeature.h"

using namespace boost;

namespace dfeat {

FeatureListRef      extractDavePoem (size_t step,const std::vector<size_t>& scales,
    float minX,float minY,float maxX,float maxY,
    const char* fileName, int marge, int size, const char* parameters)
{
    DavePoem extractor;
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

#endif
