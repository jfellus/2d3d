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
#include "Mesh.h"
#include "retin/toolbox/document/Mesh.h"
#include "retin/toolbox/core/auto_array_ptr.h"
#include "basic/Polygon.h"

#include <float.h>
#include <math.h>

namespace retin {
    
    void Mesh::computeFaceNormals() {
        faceNormals.resize(faces.size());
        
        for(size_t i = 0 ; i < faces.size() ; i++)
            faceNormals[i] = faces[i].getNormal();
    }   
    
    void Mesh::computeFaceNormalsOut() {
        faceNormals.resize(faces.size());

        for(size_t i = 0 ; i < faces.size() ; i++) {
            faceNormals[i] = faces[i].getNormal();
            if(faceNormals[i].dot(faces[i].getCenter())<0) faceNormals[i].mul(-1);
        }
    }

    void Mesh::computeVertexNormals() {
        vertexNormals.resize(vertices.size());
        
        for(size_t i = 0 ; i < faces.size() ; i++) {
            Polygon fi = faces[i];
            for(size_t j = 0 ; j < fi.vertex_indices.size() ; j++) {
                vertexNormals[fi.vertex_indices[j]].add(fi.getNormal());
            }           
        }
        
        for(size_t i = 0 ; i < vertexNormals.size() ; i++)
            vertexNormals[i].normalize();
        
    }
    
    void Mesh::computeVertexNormalsOut() {
        vertexNormals.resize(vertices.size());

        for(size_t i = 0 ; i < faces.size() ; i++) {
            Polygon fi = faces[i];
            for(size_t j = 0 ; j < fi.vertex_indices.size() ; j++) {
                vertexNormals[fi.vertex_indices[j]].add(fi.getNormal());
            }
        }

        for(size_t i = 0 ; i < vertexNormals.size() ; i++) {
            vertexNormals[i].normalize();
            if(vertexNormals[i].dot(vertices[i])<0) vertexNormals[i].mul(-1);
        }
    }

    void Mesh::setScale(float radius) {
		
		Vector3d vmin(FLT_MAX, FLT_MAX, FLT_MAX), vmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			
		for(size_t i = 0 ; i < vertices.size() ; i++) {
				// x
				if(vertices[i].x < vmin.x)
					vmin.x = vertices[i].x;
				else if(vertices[i].x > vmax.x)
					vmax.x = vertices[i].x;
				// y
				if(vertices[i].y < vmin.y)
					vmin.y = vertices[i].y;
				else if(vertices[i].y > vmax.y)
					vmax.y = vertices[i].y;
				// z
				if(vertices[i].z < vmin.z)
					vmin.z = vertices[i].z;
				else if(vertices[i].z > vmax.z)
					vmax.z = vertices[i].z;
		}
		
		Vector3d trans = vmax;
		trans.add(vmin);
		trans.div(-2);
		
		Vector3d diff = vmax;
		diff.sub(vmin);
		float scale = radius / fmax(diff.x, fmax(diff.y, diff.z));
		
		for(size_t i = 0 ; i < vertices.size() ; i++) {
			vertices[i].add(trans);
			vertices[i].mul(scale);
		}
		
	}
    


}
