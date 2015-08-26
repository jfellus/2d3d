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
 * \file NativeDaveFeatureGenerator.h
 * \author David Picard
 * \version 4.2
 */

#ifndef __retin_NativeDepthImagesFeatureGenerator_h__
#define __retin_NativeDepthImagesFeatureGenerator_h__

#include "DepthImages.h"
#include "retin/toolbox/nativegenerator/NativeFeatureListGenerator.h"
#include "retin/toolbox/document/list/FeatureList.h"

#include <vector>
#include <string>

namespace retin {
	class NativeDepthImagesFeatureGenerator : public DepthImages, public NativeFeatureListGenerator {
		
	public:                
#ifndef RETIN_DISABLE_JAVA
        virtual void initParentTable(ManagerRef manager, std::string baseName, std::string tableName);
        virtual void initParentTable(TableRef _parentTable);
        virtual std::string addressFileFromKey(std::string key);
#endif

		virtual void initParams(ParametersRef params);
		virtual void reset(){}
		virtual FeatureListRef computeFeatureListFile(std::string addressFile);
		virtual size_t getDim(ParametersRef params);
		virtual size_t getDim();
		
	};
}

#endif
