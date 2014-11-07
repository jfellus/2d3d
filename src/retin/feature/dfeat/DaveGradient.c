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

#include "DaveGradient.h"
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#define M_1_PI         (1.0/M_PI)
#endif

/*! filtre dérivateur en X */
void derivX(float* input, float* output, size_t w, size_t h) {
	size_t i/*, j*/;
	
	if(input == NULL || output == NULL)
		return;

	memcpy(output+1, input+2, (w*h-2)*sizeof(float));
	for( i = 0 ; i < h ; i++) {
/*		output[i*w+j] = input[i*w+j+1] - input[i*w+j-1]; */
/*		memcpy(output+i*w+1, input+i*w+2, (w-2)*sizeof(float));*/
		vector_sub_float(output+i*w+1, input+i*w, w-2);
	}
}

/*! filtre dérivateur en Y */
void derivY(float* input, float* output, size_t w, size_t h) {
	size_t i/*, j*/;
	
	if(input == NULL || output == NULL)
		return;
		
	memcpy(output+w, input+2*w, w*(h-2)*sizeof(float)); /* décale d'une ligne avant et après */
	for( i = 1 ; i < h-1 ; i++) {
		/* output[i*w+j] = input[(i+1)*w+j] - input[(i-1)*w+j]; */
		vector_sub_float(output+i*w, input+(i-1)*w, w);
	}
}

/*! magnitude du gradient */
void gradientMagnitude(float* dx, float* dy, float* output, size_t w, size_t h) {
	size_t i, j;
	for( i = 1 ; i < h-1 ; i++) {
		for( j = 1 ; j < w-1 ; j++) {
			float m = dx[i*w+j]*dx[i*w+j] + dy[i*w+j]*dy[i*w+j];
			output[i*w+j] = sqrt(m);
		}
	}
}

/*! orientation du gradient */
void gradientOrientation(float* dx, float* dy, float* output, size_t w, size_t h) {
	size_t i, j;
	for( i = 1 ; i < h-1 ; i++) {
		for( j = 1 ; j < w-1 ; j++) {
			float gx = dx[i*w+j];
			float gy = dy[i*w+j];
			float angle = atan2f(gy, gx);
			mod2pi(angle);
			output[i*w+j] = angle / (2.0f * M_PI);
		}
	}
}

/*! orientation du gradient */
void gradientOrientationMagnitude(float* dx, float* dy, float* output, size_t w, size_t h) {
        
	size_t i, j;
         float *imgx, *imgy;        
	imgx = malloc(w*sizeof(float)); 
	imgy = malloc(w*sizeof(float));	
                
	for( i = 1 ; i < h-1 ; i++) {
				
		//norme en x
		memcpy(imgx, dx+i*w, w*sizeof(float));
		vector_mul_float(imgx, dx+i*w, w);
		
		//norme en Y
		memcpy(imgy, dy+i*w, w*sizeof(float));
		vector_mul_float(imgy, dy+i*w, w);
		
                
		//norme tot
		vector_add_float(imgy, imgx, w);
		vector_sqrt_float(imgy, w);
	
		for( j = 1 ; j < w-1 ; j++) {
			float gx = dx[i*w+j];
			float gy = dy[i*w+j];  
                        float angle = 0;
                        //if(gy != 0)
                                angle = atan2f(gy, gx);
                            
			mod2pi(angle);
    
 
			angle = angle * 4 * M_1_PI;
			
			float m = imgy[j];

/*			size_t f = (size_t) (((int)floorf(angle)) % 8);
			float rf = angle - f;
                        				
			output[f*w*h         + i*w+j] = (1-rf) * m;
			output[((f+1)%8)*w*h + i*w+j] =    rf  * m;
*/
/*				float m = sqrtf(gx*gx+gy*gy);
*/			
				size_t f = (size_t) ((int)floorf(angle) % 8);
				float rf = angle - f;
				
				output[f*w*h         + i*w+j] = (1-rf) * m;
				output[((f+1)%8)*w*h + i*w+j] =    rf  * m;
/*			}
 */

		}
	}
	free(imgx);
	free(imgy);

}


/*! orientation du gradient */
void gradientNbOrientationMagnitude(float* dx, float* dy, float* output, size_t w, size_t h, size_t nb_orientation) {
        
	size_t i, j;
         float *imgx, *imgy;        
	imgx = malloc(w*sizeof(float)); 
	imgy = malloc(w*sizeof(float));	
                
	for( i = 1 ; i < h-1 ; i++) {
				
		//norme en x
		memcpy(imgx, dx+i*w, w*sizeof(float));
		vector_mul_float(imgx, dx+i*w, w);
		
		//norme en Y
		memcpy(imgy, dy+i*w, w*sizeof(float));
		vector_mul_float(imgy, dy+i*w, w);
		
                
		//norme tot
		vector_add_float(imgy, imgx, w);
		vector_sqrt_float(imgy, w);
	
		for( j = 1 ; j < w-1 ; j++) {
			float gx = dx[i*w+j];
			float gy = dy[i*w+j];  
                        float angle = 0;
                        //if(gy != 0)
                                angle = atan2f(gy, gx);
                            
			mod2pi(angle);
    
 
			angle = angle * nb_orientation / 2.0f * M_1_PI;
			
			float m = imgy[j];
				size_t f = (size_t) ((int)floorf(angle) % nb_orientation);
				float rf = angle - f;
				
				output[f*w*h         + i*w+j] = (1-rf) * m;
				output[((f+1)%nb_orientation)*w*h + i*w+j] =    rf  * m;
		}
	}
	free(imgx);
	free(imgy);

}

