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
