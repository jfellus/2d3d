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

#ifdef DRETIN_ENABLE_OSMESA

#include "Renderer3D.h"
#include <iostream>

using namespace std;


static GLfloat MAT_SPECULAR[] = { .1, .1, .1, .1 };
static GLfloat MAT_SHININESS[] = { 200 };
static GLfloat LIGHT_POSITION[] = { 1.0, 1.0, 1.0, 0.0 };
static GLfloat LIGHT_POSITION2[] = { -1.5, 0, 0.7, 0.0 };
static GLfloat LIGHT_AMBIENT[] = { 0.2,0.2,0.2,0.2};
static GLfloat LIGHT_DIFFUSE[] = { 0.7,0.7,0.7,0.7};

Renderer3D::Renderer3D(size_t w, size_t h) {
	this->w = w;
	this->h = h;
	buffer = new uint8_t[w*h*4];
	bRenderNormals = false;

	context = OSMesaCreateContextExt( OSMESA_ARGB, 16, 8, 16, NULL );
	if(!context) throw "Can't create OSMesaContext";

	if(OSMesaMakeCurrent(context, buffer, GL_UNSIGNED_BYTE, w, h) == GL_FALSE)
		throw "Can't associate OSMesaContext to buffer";

	init3d();
}

Renderer3D::~Renderer3D() {
	OSMesaDestroyContext(context);
	delete buffer;
}

void Renderer3D::setCamera(Vector3d cam) {
	this->cam = Vector3d(cam.x, cam.y, cam.z);
}

void Renderer3D::renderNormals(Mesh& mesh) {
	glColor3f(1.0f,0,0);
	for(size_t i = 0; i < mesh.nb_faces; i++) {
		if(i%100!=0) continue;
		glBegin(GL_LINES);
		for(size_t j = 0; j < mesh.faces[i].size(); j++) {
			glVertex3d(mesh.vertices[mesh.faces[i].vertex_indices[j]].x,
					mesh.vertices[mesh.faces[i].vertex_indices[j]].y,
					mesh.vertices[mesh.faces[i].vertex_indices[j]].z);
			glVertex3d(
					mesh.vertices[mesh.faces[i].vertex_indices[j]].x + 0.05*mesh.faceNormals[i].x,
					mesh.vertices[mesh.faces[i].vertex_indices[j]].y + 0.05*mesh.faceNormals[i].y,
					mesh.vertices[mesh.faces[i].vertex_indices[j]].z + 0.05*mesh.faceNormals[i].z);
		}
		glEnd();
	}
}

void Renderer3D::renderMesh(Mesh& mesh) {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor3f(0.9,0.8,0.6);
	for(size_t i = 0; i < mesh.nb_faces; i++) {
		glBegin(GL_POLYGON);
		for(size_t j = 0; j < mesh.faces[i].size(); j++) {
			glNormal3d(mesh.faceNormals[i].x, mesh.faceNormals[i].y,mesh.faceNormals[i].z);
			glVertex3d(mesh.vertices[mesh.faces[i].vertex_indices[j]].x,
					mesh.vertices[mesh.faces[i].vertex_indices[j]].y,
					mesh.vertices[mesh.faces[i].vertex_indices[j]].z);
		}
		glEnd();
	}

	if(bRenderNormals) renderNormals(mesh);

}

void Renderer3D::init3d() {
	glClearColor (1, 1, 1, 1);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
	glDepthFunc(GL_LEQUAL);                         // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspective
	glDisable(GL_CULL_FACE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MAT_SPECULAR);
	glMaterialfv(GL_FRONT, GL_SHININESS,MAT_SHININESS);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(45, (GLfloat) w/(GLfloat) h, 0.01, 100.0);
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
}

void Renderer3D::render(Mesh& mesh, float* img) {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam.x, cam.y, cam.z,	0, 0, 0,  0, -1, 0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_POSITION2);

	glPushMatrix();
	renderMesh(mesh);
	glPopMatrix();


	int w, h;
	int bpv;
	uint8_t *buf;
	// OSMesaGetDepthBuffer(context, &w, &h, &bpv, &buf);
	OSMesaGetColorBuffer(context, &w, &h, &bpv, (void**)&buf);

	cout << "w=" << w << ", h="<<h<<"\n";

	for(size_t i = 0 ; i < (size_t)w*h ; i++){
		img[i*3] = buf[i*4+1];
		img[i*3+1] = buf[i*4+2];
		img[i*3+2] = buf[i*4+3];
	}

}

#endif
