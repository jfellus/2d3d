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
#ifndef __DaveGradient_h__
#define __DaveGradient_h__


#include "retin/toolbox/algebra/vector_float.h"
#include "retin/toolbox/algebra/matrix_float.h"

#ifdef __cplusplus
extern "C" {
#endif

#define mod2pi(x) { while(x > 2*M_PI) x -= 2.*M_PI; while(x < 0) x += 2.*M_PI; }

/*! filtre derivateur en X */
void derivX(float* input, float* output, size_t w, size_t h);

/*! filtre derivateur en X */
void derivY(float* input, float* output, size_t w, size_t h);

/*! magnitude du gradient */
void gradientMagnitude(float* dx, float* dy, float* output, size_t w, size_t h);

/*! orientation du gradient */
void gradientOrientation(float* dx, float* dy, float* output, size_t w, size_t h);

/*! orientation du gradient */
void gradientOrientationMagnitude(float* dx, float* dy, float* output, size_t w, size_t h);

/*! orientation du gradient */
void gradientNbOrientationMagnitude(float* dx, float* dy, float* output, size_t w, size_t h, size_t nb_orientation);

#ifdef __cplusplus
}
#endif


#endif
