/* 
 * File:   Codec3DPLY.h
 * Author: jfellus
 *
 * Created on 17 octobre 2012, 11:44
 */

#ifndef CODEC3DPLY_H
#define	CODEC3DPLY_H

#include "Codec3D.h"
#include <stdio.h>
#include "ply/ply.h"
#include <string>

namespace retin {
using std::string;
using std::ifstream;

//! decoder au format ply
class Decoder3DPLY : public Decoder3D {
private:
	FILE* fptr;
	size_t nb_vertices, nb_faces;


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



#endif	/* CODEC3DPLY_H */

