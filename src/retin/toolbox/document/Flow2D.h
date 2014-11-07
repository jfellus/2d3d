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

