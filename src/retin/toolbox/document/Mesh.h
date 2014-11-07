/* 
 * File:   Mesh.h
 * Author: heditabi
 *
 * Created on 17 octobre 2012, 11:48
 */

#ifndef MESH_H
#define	MESH_H

#include "retin/toolbox/document/basic/Vector3d.h"
#include "retin/toolbox/document/basic/Polygon.h"

namespace retin {
    
    class Mesh {
    public:
        auto_array_ptr<Vector3d> vertices;
        auto_array_ptr<Polygon> faces;
        auto_array_ptr<Vector3d> vertexNormals;
        auto_array_ptr<Vector3d> faceNormals;
        auto_array_ptr<Polygon> edges;
        
        size_t nb_vertices, nb_faces;
        
        void computeFaceNormals();
        void computeVertexNormals();
        void computeVertexNormalsOut();
        void computeFaceNormalsOut();
        void setScale(float radius = 1.0);
    };
    
    
    
    
}


#endif	/* MESH_H */

