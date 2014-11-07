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
