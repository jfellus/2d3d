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

#ifndef RENDERER3D_H_
#define RENDERER3D_H_

#ifdef RETIN_ENABLE_OSMESA

#include "retin/toolbox/codec3d/Codec3DOFF.h"
#include "retin/toolbox/codec3d/Codec3DPLY.h"
#include <GL/osmesa.h>
#include <GL/glu.h>


using namespace retin;


class Renderer3D {
private:
	Vector3d cam;
	size_t w,h;

	uint8_t* buffer;
	OSMesaContext context;

	bool bRenderNormals;
public:
	Renderer3D(size_t w, size_t h);
	virtual ~Renderer3D();

	void setCamera(Vector3d cam);
	void setRenderNormals(bool bRenderNormals) {this->bRenderNormals = bRenderNormals;}

	void render(Mesh& mesh, float* img);

protected:
	void renderMesh(Mesh& mesh);
	void renderNormals(Mesh& mesh);
	void init3d();
};

#endif

#endif /* RENDERER3D_H_ */
