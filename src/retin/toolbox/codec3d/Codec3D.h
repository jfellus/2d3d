/* 
 * File:   Codec3D.h
 * Author: heditabi
 *
 * Created on 17 octobre 2012, 11:39
 */

#ifndef CODEC3D_H
#define	CODEC3D_H

#include <string>
#include <fstream>

#include "retin/toolbox/document/Mesh.h"

using std::string;
using std::ifstream;

namespace retin {
    
    //! classe mère des codec 3D
    class Codec3D {
        string	strFilename;
	FILE*		file;
        size_t          nb_vertex, nb_faces;
        
        Mesh read_mesh(string filename);
        
    };
    
    
    //! classe mere des encodeur 3D
    class Encoder3D : public Codec3D {
        
    };
    
    //! classe mere des decodeurs 3D
    class Decoder3D : public Codec3D {        
        public:

                //! Ouvre le fichier en lecture et prépare le décodage.
        virtual	void		read_open (const std::string& filename) = 0;
                //! Lit l'entête du fichier (type, taille, etc.).
        virtual	void		read_header () = 0;
                //! Lit les données.
        virtual	void		read_data (Mesh *mesh) = 0;
                //! Ferme le fichier et détruit les données temporaires.
        virtual	void		read_close () = 0;
        
    };
    
    void read_mesh(Mesh *mesh, string filename);
    
}


#endif	/* CODEC3D_H */

