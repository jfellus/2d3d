/*
 * Renderer3D.h
 *
 *  Created on: 30 oct. 2013
 *      Author: jfellus
 */

#ifndef RENDERER3D_H_
#define RENDERER3D_H_


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

#endif /* RENDERER3D_H_ */
