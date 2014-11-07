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
#ifndef __DavePoem_h__
#define __DavePoem_h__

#include "AbstractDaveFeature.h"

namespace dfeat {

    //! dense sampling poem 
    class DavePoem : public AbstractDaveFeature {
        size_t nb_descs;
        size_t scale;
        size_t radius;
        size_t neighbors;
        size_t nb_orientation;
        size_t nb_cell;
        size_t desc_size;
        retin::auto_array_ptr<float> magnitude;
        retin::auto_array_ptr<float> descs;

        void computeMagnitudeOrientation(float *gx = NULL, float *gy = NULL);
        void computeSize() {
			size_t nb_bins = 1<<neighbors;
			desc_size = nb_bins*nb_cell*nb_cell*nb_orientation;
		}

    public:
		DavePoem() : scale(5), radius(7), neighbors(6), nb_orientation(3), nb_cell(1) {
			computeSize();
		}
		virtual ~DavePoem() {}
        virtual void clear();
        
        void setScale(size_t s) { scale = s; computeSize(); }
        void setRadius(size_t r) { radius = r; computeSize(); }
        void setNeighbors(size_t n) { neighbors = n; computeSize(); }
        void setOrientation(size_t o) { nb_orientation = o; computeSize(); }
        void setCell(size_t c) { nb_cell = c; computeSize(); }

		virtual void extract(size_t step, size_t scale) {
			extract(step, scale, NULL, NULL);
		}
        virtual void extract(size_t step, size_t scale, float *gx, float *gy);

        virtual size_t getKeypointCount() const { return nb_descs; }
        virtual size_t getDescriptorSize() const { return desc_size; }
        virtual const float* getDescriptors() const { return descs.get(); }

    };

}

#endif
