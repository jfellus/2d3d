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

