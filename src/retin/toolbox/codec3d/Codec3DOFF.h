/* 
 * File:   Codec3DOFF.h
 * Author: heditabi
 *
 * Created on 17 octobre 2012, 11:44
 */

#ifndef CODEC3DOFF_H
#define	CODEC3DOFF_H

#include "Codec3D.h"

namespace retin {
        
    //! decoder au format off
    class Decoder3DOFF : public Decoder3D {
    private:
        ifstream fptr;
        size_t nb_vertices, nb_faces;
	string str;
        
                
    public:

                //! Ouvre le fichier en lecture et prépare le décodage.
        void		read_open (const std::string& filename);
                //! Lit l'entête du fichier (type, taille, etc.).
        void		read_header ();
                //! Lit les données.
        void		read_data (Mesh *mesh);
                //! Ferme le fichier et détruit les données temporaires.
        void		read_close ();
    };
    
}



#endif	/* CODEC3DOFF_H */

