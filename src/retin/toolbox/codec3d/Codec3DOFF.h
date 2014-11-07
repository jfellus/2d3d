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
