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
/**
 * \file Flow2D.h
 * \author Romain Negrel
 * \version 4.0
 */

#ifndef __retin_Flow2D_h__
#define __retin_Flow2D_h__

#include "retin/toolbox/document/Document.h"
#include "retin/toolbox/core/auto_array_ptr.h"
#include <iostream>
#include <fstream>

namespace retin {

    class Flow2D : public Document {
        RETIN_SERIALIZABLE_OBJECT_DECLARATIONS(retin / toolbox / document, Flow2D);

        retin::auto_array_ptr<jfloat> pFlowX;
        retin::auto_array_ptr<jfloat> pFlowY;
        jint nWidth, nHeight;

        template<class SerializerType>
        friend void serialize(SerializerType& s, Flow2D& x) {
            s.object("width", x.nWidth);
            s.object("height", x.nHeight);
            s.matrix("flowX", x.pFlowX, x.nWidth, x.nHeight);
            s.matrix("flowY", x.pFlowY, x.nWidth, x.nHeight);
        }

    public:

        Flow2D(int nWidth = 1, int nHeight = 1) :
        pFlowX(nWidth*nHeight), pFlowY(nWidth*nHeight), nWidth(nWidth), nHeight(nHeight) {
        }

        jfloat* getDataX() {
            return pFlowX.get();
        }

        jfloat* getDataY() {
            return pFlowY.get();
        }

        void setDataX(jfloat* p) {
            pFlowX.set(p);
        }

        void setDataY(jfloat* p) {
            pFlowY.set(p);
        }

        jint getWidth() const {
            return nWidth;
        }

        jint getHeight() const {
            return nHeight;
        }

        void resize(int nWidth = 1, int nHeight = 1);

        void setValueX(int i, int j, jfloat val) {
            pFlowX[i + j * nWidth] = val;
        }

        jfloat getValueX(int i, int j) {
            return pFlowX[i + j * nWidth];
        }

        void setValueY(int i, int j, jfloat val) {
            pFlowY[i + j * nWidth] = val;
        }

        jfloat getValueY(int i, int j) {
            return pFlowY[i + j * nWidth];
        }

        void gnuplot_write(std::string & file_path_name) {
            std::ofstream output(file_path_name.c_str(), std::ios::out);
            if (!output) {
                std::cout << "GNUPLOT_WRITE error in opening the file " << file_path_name << std::endl;
            } else {
                for (int i = 0; i < nWidth; i++) {
                    for (int j = 0; j < nHeight; j++) {
                        output << i << "\t" << j << "\t" << this->getValueX(i, j)<< "\t" << this->getValueY(i, j) << "\n";
                    }
                }
            }
            output.close();
        }

        void threshold(double th) {
            for (int i = 0; i < nWidth; i++) {
                for (int j = 0; j < nHeight; j++) {
                    if( (this->getValueX(i,j)*this->getValueX(i,j)) + (this->getValueY(i,j)*this->getValueY(i,j)) < th ){
                    this->setValueX(i,j,0.0) ;
                    this->setValueY(i,j,0.0) ; 
                    }
                }
            }
        }
    };

    typedef boost::shared_ptr<Flow2D> Flow2DRef;

}

#endif
