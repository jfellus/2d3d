#include "Polygon.h"
#include "retin/toolbox/core/auto_array_ptr.h"


namespace retin {
    
    //! TODO: ultra buggé, ne pas utiliser
    void Polygon::addVertex(size_t index, Vector3d v) {
        size_t s = vertices.size();
        
        //copy vertices and indexes
        auto_array_ptr<Vector3d> cv(vertices);
        auto_array_ptr<size_t> cvi(vertex_indices);
        
        vertices.resize(s+1);
        vertex_indices.resize(s+1);
        
        vertices.copy(cv);
        vertices[s] = v;
        vertex_indices.copy(cvi);
        vertex_indices[s] = index;
    }
    
    void Polygon::setVertex(size_t i, size_t index, Vector3d v) {
        if((i >= vertices.size()) || (i >= vertex_indices.size()))
            return;
        
        vertices[i] = v;
        vertex_indices[i] = index;
    }
    
    Vector3d Polygon::getNormal() {
        
        if(vertices.size() <= 2)
            return Vector3d(0, 0, 0);
                
        Vector3d V4, V5, V6;

	V4.x = vertices[1].x - vertices[0].x;
	V4.y = vertices[1].y - vertices[0].y;
	V4.z = vertices[1].z - vertices[0].z;

	V5.x = vertices[2].x - vertices[0].x;
	V5.y = vertices[2].y - vertices[0].y;
	V5.z = vertices[2].z - vertices[0].z;

	V6 = crossProduct(V4,V5);
        V6.normalize();
        
	return(V6 );
        
    }

    Vector3d Polygon::getCenter() {
    	Vector3d center(0,0,0);
    	for(int i=0; i<vertices.size(); i++)
    		center.add(vertices[i]);
    	center.mul(1.0/vertices.size());
    	return center;
    }
}
