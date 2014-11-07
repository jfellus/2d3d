#include "Vector3d.h"


namespace retin {
    
    Vector3d crossProduct(Vector3d V1, Vector3d V2) {
        Vector3d v3(0, 0, 0);
        
        v3.x = V1.y*V2.z - V2.y*V1.z;
	v3.y = V1.z*V2.x - V2.z*V1.x;
	v3.z = V1.x*V2.y - V2.x*V1.y;
        
        return v3;
    }
    
    
    
}
