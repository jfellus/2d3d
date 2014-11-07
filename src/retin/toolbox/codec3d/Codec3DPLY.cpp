#include <stdint.h>
#include "Codec3DPLY.h"
#include "ply/ply.h"
#include "retin/toolbox/document/Mesh.h"
#include "retin/toolbox/document/basic/Polygon.h"

#include <stdio.h>
#include <stdlib.h>

#include <sstream>
#include <iostream>

using namespace std;

#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace retin {


typedef struct {
	float x,y,z;
} ply_vertex;
typedef struct {
	uint8_t nvertices; int32_t* vertex_indices;
} ply_face;

static PlyProperty WANTED_VERTEX_PROPERTIES[] = {
		{"x", Float32, Float32, offsetof(ply_vertex, x), 0, 0, 0, 0},
		{"y", Float32, Float32, offsetof(ply_vertex, y), 0, 0, 0, 0},
		{"z", Float32, Float32, offsetof(ply_vertex, z), 0, 0, 0, 0}
};
static PlyProperty WANTED_FACE_PROPERTIES[] = {
		{"vertex_indices", Int32, Int32, offsetof(ply_face,vertex_indices),  1, Uint8, Uint8, offsetof(ply_face,nvertices)}
};


void Decoder3DPLY::read_open (const std::string& filename) {
	fptr = fopen(filename.c_str(), "r");
	if(!fptr) {
		std::cout << "Can't open " << filename << "\n";
		throw "can't open ply file";
	}
}

void Decoder3DPLY::read_header() {
}

void Decoder3DPLY::read_data(Mesh *mesh) {
	if(mesh == NULL) {
		std::cout << "mesh is null, abort read_data()" << std::endl;
		return;
	}

	PlyFile *in_ply = read_ply (fptr);

	/* examine each element type that is in the file (vertex, face) */
	for (int i = 0; i < in_ply->num_elem_types; i++) {
		int elem_count;
		char *elem_name = setup_element_read_ply (in_ply, i, &elem_count);

		/* Read vertices */
		if (!strcmp("vertex", elem_name)) {
			mesh->nb_vertices = elem_count;
			mesh->vertices.resize(mesh->nb_vertices);

			/* set up for getting vertex elements (we want x,y,z) */
			setup_property_ply (in_ply, &WANTED_VERTEX_PROPERTIES[0]);
			setup_property_ply (in_ply, &WANTED_VERTEX_PROPERTIES[1]);
			setup_property_ply (in_ply, &WANTED_VERTEX_PROPERTIES[2]);

			/* grab the vertex elements and store them in our list */
			ply_vertex pv;
			for (int j = 0; j < elem_count; j++) {
				get_element_ply (in_ply, (void *) &pv);
				Vector3d v(pv.y, pv.x, pv.z);
				mesh->vertices[j] = v;
			}
		}

		/* Read faces */
		else if (equal_strings ("face", elem_name)) {
			mesh->nb_faces = elem_count;
			mesh->faces.resize(mesh->nb_faces);

			setup_property_ply (in_ply, WANTED_FACE_PROPERTIES);

			/* grab all the face elements and place them in our list */
			for (int j = 0; j < elem_count; j++) {
				ply_face pf;
				get_element_ply (in_ply, (void *) &pf);
				Polygon p(pf.nvertices);
				for(uint8_t k=0; k < pf.nvertices; k++) {
					p.setVertex(k, pf.vertex_indices[k], mesh->vertices[pf.vertex_indices[k]]);
				}
				mesh->faces[j] = p;
			}
		}
		else  get_other_element_ply (in_ply);
	}

	close_ply(in_ply);
	free_ply(in_ply);

	//compute normals
	mesh->computeFaceNormals();
	mesh->computeVertexNormals();
}

void Decoder3DPLY::read_close() {
	fclose(fptr);
}

}
