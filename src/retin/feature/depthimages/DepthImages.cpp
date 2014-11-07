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

#include "DepthImages.h"
#include "DepthCamera.h"
#include "retin/feature/dfeat/DaveHog.h"
#include "retin/toolbox/codec3d/Codec3DOFF.h"
#include "retin/feature/color/ImageProcessing.h"
#include <GL/osmesa.h>
#include <GL/glu.h>
#include <iostream>

using namespace dfeat;
using namespace retin;
using namespace std;

// renvoie l'image relative à la caméra cam
void DepthImages::generateDepthImage(auto_array_ptr<float>& image,Vector3d cam, Mesh& mesh) {

	//BytesMatrixRef matrix = make_shared<BytesMatrix>(width, height);
	image.resize(width*height);
	
	// create context
	OSMesaContext context = OSMesaCreateContextExt( OSMESA_ARGB, 16, 8, 16, NULL );
	if(!context)
		return;
	//cout << "context ok" << endl;
	
	auto_array_ptr<unsigned int> buffer(width*height);
	if(OSMesaMakeCurrent(context, buffer.get(), GL_UNSIGNED_BYTE, width, height) == GL_FALSE)
		return;
		
	//cout << "make current ok" << endl;
	
	// scale mesh
	mesh.setScale(0.72f);
	
    	
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
    glDepthFunc(GL_LESS);                         // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspective 
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(90, (GLfloat) width/(GLfloat) height, 0.01, 2.0);
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(cam.x, cam.y, cam.z, // from cam
				0, 0, 0, // to center
				0, 1, 0); // y is up

	//cout << "GL init ok" << endl;

	// draw object in GL
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glColor3f(0.5f,0.5f,0.5f);
	for(size_t i = 0; i < mesh.nb_faces; i++) {
		size_t f_size = mesh.faces[i].size();
		glBegin(GL_POLYGON);
		for(size_t j = 0; j < f_size; j++) {
			glVertex3d(mesh.vertices[mesh.faces[i].vertex_indices[j]].x, 
				mesh.vertices[mesh.faces[i].vertex_indices[j]].y, 
				mesh.vertices[mesh.faces[i].vertex_indices[j]].z);
		}
		glEnd();
		glBegin(GL_POLYGON);
		for(size_t j = 0 ; j < f_size; j++) {
			glVertex3d(mesh.vertices[mesh.faces[i].vertex_indices[f_size-1-j]].x, 
				mesh.vertices[mesh.faces[i].vertex_indices[f_size-1-j]].y, 
				mesh.vertices[mesh.faces[i].vertex_indices[f_size-1-j]].z);
		}
		glEnd();
	}
	glPopMatrix();
	//cout << "GL draw ok" << endl;

	int w, h;
	int bpv;
	void *buf;
	OSMesaGetDepthBuffer(context, &w, &h, &bpv, &buf);
	//OSMesaGetColorBuffer(context, &w, &h, &bpv, &buf);
	for(size_t i = 0 ; i < (size_t)w*h ; i++) {
		image[i] = 255 - (255.f/65635.f * ((float)((unsigned short*)buf)[i]));
		//image[i] = ((float)(((unsigned int*)buf)[i] &&  0xFF));
	}

	buffer.clear();
	OSMesaDestroyContext(context);
	
}


void DepthImages::extractDescriptors(float*& descs, std::string file, size_t& feature_count) {
	Decoder3DOFF dec;
    Mesh mesh;
    dec.read_open(file);
    dec.read_header();
    dec.read_data(&mesh);

    // extract dhog
    feature_count = 0;
    descs = NULL;
    vector<auto_array_ptr<float> > descriptors;
    size_t featureDim = 0;
    if(nb_cams < 1)
		nb_cams = 1;
	auto_array_ptr<float> input(width*height);
	for(size_t n = 0 ; n < nb_cams ; n++) {
		Vector3d cam(DEPTH_IMAGE_CAMERAS[n%200][0], 
					 DEPTH_IMAGE_CAMERAS[n%200][1],
					 DEPTH_IMAGE_CAMERAS[n%200][2]);
		cam.normalize();

		generateDepthImage(input, cam, mesh);

		DaveHog extractor;
		// equalize the resulting image
		equalize(input.get(),width*height,1);
		extractor.setInput(input.get(), width, height);
		//extractor.cropInput(minX,minY,maxX,maxY);
		float * buffer = NULL;
		featureDim = extractor.getDescriptorSize();
		size_t count;
		dfeat::extractDaveFeature (buffer, count, extractor, step, scales);
		
		auto_array_ptr<float> b(buffer, count*featureDim, true);
		descriptors.push_back(b);
		feature_count += count;
	}
	input.clear();
	
	auto_array_ptr<float> total(feature_count*featureDim);
	size_t t = 0;
	for(size_t n = 0 ; n < nb_cams ; n++) {
		size_t si = descriptors[n].size();
		memcpy(total.get(t), descriptors[n].get(), si*sizeof(float));
		t += si;
		descriptors[n].clear();
	}
	
	descs = total.release();
}
