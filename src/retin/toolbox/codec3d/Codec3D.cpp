#include <stdint.h>
#include "Codec3D.h"

#include "Codec3DOFF.h"

namespace retin {

void read_mesh(Mesh *mesh, string filename) {
     
    Decoder3DOFF dec;
    dec.read_open(filename);
    dec.read_header();
    dec.read_data(mesh);
    dec.read_close();
}


}
