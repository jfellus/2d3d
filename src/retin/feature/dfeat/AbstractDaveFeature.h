/*
Copyright © CNRS 2012. 
Authors: Philippe-Henri Gosselin, David Picard, Romain Négrel
Contact: philippe-henri.gosselin@ensea.fr

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
#ifndef __AbstractDaveFeature_h__
#define __AbstractDaveFeature_h__

#include "retin/toolbox/core/core.h"
#include "retin/toolbox/core/auto_array_ptr.h"

namespace dfeat {

    class AbstractDaveFeature {
    protected:
        size_t width, height;
        unsigned char* image,*palette;
        float* input;


    public:
        AbstractDaveFeature();
        virtual ~AbstractDaveFeature();
        virtual void clear();

        //! Warning : takes ownership of in
        void                    setInput(float* in, size_t w, size_t h);

        virtual void            setFileInput (const char* fileName, int maxSize = 0);

        void                    setFileInput (const char* fileName,const char* parameters);

        //! Sous image [minX,maxX[ \times [minY,maxY[
        void                    cropInput(float minX, float minY, float maxX, float maxY, int marge = 0);
        //! Sous image [minX,maxX[ \times [minY,maxY[
        void                    cropInput(int minX, int minY, int maxX, int maxY);

        virtual void            extract(size_t step, size_t scale) = 0;

        size_t                  getWidth() const { return width; }
        size_t                  getHeight() const { return height;  }
        virtual size_t          getKeypointCount() const = 0;
        virtual size_t          getDescriptorSize() const = 0;
        virtual const float*    getDescriptors() const = 0;

    };

    void      extractDaveFeature (float*& descriptors,size_t& descriptorCount,AbstractDaveFeature& extractor,        
        size_t step,const std::vector<size_t>& scales);

}

#endif
