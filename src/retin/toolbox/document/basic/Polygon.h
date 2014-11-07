/* 
 * File:   Polygon.h
 * Author: heditabi
 *
 * Created on 17 octobre 2012, 12:00
 */

#ifndef POLYGON_H
#define	POLYGON_H


#include "retin/toolbox/document/basic/Vector3d.h"

namespace retin {
    
    class Polygon {
    public :
        auto_array_ptr<size_t> vertex_indices;
        auto_array_ptr<Vector3d> vertices;
        
        Polygon(size_t degree = 0) { 
            vertex_indices.resize(degree);
            vertices.resize(degree);
        }
        
        //! ajoute un vertex au polygone
        void addVertex(size_t index, Vector3d v);
        //! modifie le ième vertex du polygone
        void setVertex(size_t i, size_t index, Vector3d v);
        //! renvoie la normale au plan des trois premiers vertex
        Vector3d getNormal();
        
        //! taille du polygone
        size_t size() { return vertices.size(); }
        
        Vector3d getCenter();
    };    
}


#endif	/* POLYGON_H */

