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
#include "Codec3DOFF.h"
#include "retin/toolbox/document/Mesh.h"
#include "retin/toolbox/document/basic/Polygon.h"

#include <stdio.h>
#include <stdlib.h>

#include <sstream>


namespace retin {
    
    void Decoder3DOFF::read_open (const std::string& filename) {
        fptr.open(filename.c_str(), ifstream::in);
        if(!fptr.is_open())
            std::cout << "erreur lecture " << filename << std::endl;
        
    }
    
    void Decoder3DOFF::read_header() {
        
		getline(fptr, str);

			// entete OFF
		if (strncmp(str.c_str(), "OFF" , 3))
		{
			printf("The file is not a valid OFF one ");    
		}

		getline(fptr, str);
		std::stringstream stream(str);
		stream >> nb_vertices;
		stream >> nb_faces;
    }
    
    void Decoder3DOFF::read_data(Mesh *mesh) {
        
        if(mesh == NULL) {
            std::cout << "mesh is null, abort read_data()" << std::endl;
            return;
        }
            
        mesh->nb_vertices = nb_vertices;
        mesh->vertices.resize(nb_vertices);
        mesh->nb_faces = nb_faces;
        mesh->faces.resize(nb_faces);
        
		// read vertices
		for (size_t i = 0; i< nb_vertices ; i++)
		{
			getline(fptr, str);
			std::stringstream stream(str);
			float x, y, z;
			stream >> x;
			stream >> y;
			stream >> z;
			Vector3d v(x, y, z);
			mesh->vertices[i] = v;
		}
		
		//read faces
		for (size_t i = 0; i< nb_faces ; i++)
		{
			getline(fptr, str);
			std::stringstream stream(str);

			size_t nb_v;
			stream >> nb_v;
					Polygon p(nb_v);                
			for (size_t j = 0; j< nb_v ; j++)
			{
				size_t index;
				stream >> index;
							p.setVertex(j, index, mesh->vertices[index]);
			}
					mesh->faces[i] = p;

		}
		
		//compute normals
		mesh->computeFaceNormals();
		mesh->computeVertexNormals();
			
    }
    
    void Decoder3DOFF::read_close() {
        fptr.close();
    }
    
}
