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
/* 
 * File:   Vector3d.h
 * Author: heditabi
 *
 * Created on 17 octobre 2012, 11:51
 */

#ifndef VECTOR3D_H
#define	VECTOR3D_H

#include "retin/toolbox/core/auto_array_ptr.h"

#include <math.h>

namespace retin {
    
    class Vector3d {
    public:
        float x, y, z;        
        Vector3d(float xx = 0, float yy = 0, float zz = 0) {
            x = xx;
            y = yy;
            z = zz;
        }
        
        float getNorm() {
            return sqrt(x*x+y*y+z*z);
        }
        
        void normalize() {
            float n = getNorm() + 1e-7;
            x = x/n;
            y = y/n;
            z = z/n;
        }
        
        void add(Vector3d v) {
            x += v.x;
            y += v.y;
            z += v.z;
        }
        
        void sub(Vector3d v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }
        
        float dot(Vector3d v) {
            return x*v.x+y*v.y+z*v.z;
        }
        
        float dist_l2(Vector3d v) {
            return sqrt( (v.x - x)*(v.x - x)
                        +(v.y - y)*(v.y - y)
                        +(v.z - z)*(v.z - z) );
        }
        
        void mul(float s) {
			x *= s;
			y *=s;
			z *=s;			
		}
        
        void div(float s) {
			if(s == 0)
				return;
			x /= s;
			y /= s;
			z /= s;
		}
        
    };
    
    Vector3d crossProduct(Vector3d v1, Vector3d v2);
    
}


#endif	/* VECTOR3D_H */
