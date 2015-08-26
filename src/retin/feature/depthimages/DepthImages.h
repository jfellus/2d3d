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

#ifndef _DEPTHIMAGES_H_
#define _DEPTHIMAGES_H_

#include "retin/toolbox/document/Mesh.h"

#include <vector>

using namespace std;

namespace retin {
		
	class DepthImages {

		protected:
		Mesh camera;
		size_t nb_cams;
		std::string fileDir;
		
		float* input;
		size_t height, width;
                
		vector<size_t> scales;
		size_t step;
				
		
		public:
		
		DepthImages(): nb_cams(50),
                    fileDir(""),
                    height(512),width(512),
                    step(6)
                    {
						scales.push_back(16);
					}
		virtual ~DepthImages() { scales.clear(); }
                
		virtual void reset(){}
		virtual void extractDescriptors(float*&, std::string, size_t&);
		
		
		// revoie les images de profondeurs concaténées
		//BytesMatrixRef generateDepthImages();
		
		// renvoie l'image relative à la caméra cam
		void generateDepthImage(auto_array_ptr<float>& image, Vector3d cam, Mesh& mesh);
                
		// passage par setParams() obligatoire
		void setHeight(size_t h) { height = h; }
		void setWidth(size_t w) { width = w; }
		void setNbCams(size_t n) { nb_cams = n; }
		void setStep(size_t s) { step = s; }
		void setScales(vector<size_t>& s) { 
				scales.clear();
				for(size_t i = 0 ; i < s.size() ; i++)
					scales.push_back(s[i]);
		}
	};
}



////////////////////////////////////////////////////////////////////////////////

#ifndef RETIN_DISABLE_JAVA

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL Java_retin_feature_depthimages_DepthImagesGenerator_newIntance
  (JNIEnv *, jobject);
JNIEXPORT void JNICALL Java_retin_feature_depthimages_DepthImagesGenerator_deleteIntance
  (JNIEnv *, jobject);

}

#endif

#endif
