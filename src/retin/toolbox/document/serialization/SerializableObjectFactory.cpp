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
 * \file SerializableObjectFactory.cpp
 * \author Philippe H. Gosselin
 * \version 4.0
 */

#include "retin/toolbox/document/serialization/SerializableObjectFactory.h"

#include "../basic/Integer.h"
#include "../basic/Float.h"
#include "../basic/Double.h"
#include "../basic/String.h"
#include "../basic/Text.h"
#include "../basic/Indexes.h"
#include "../basic/Feature.h"
#include "../basic/Keypoint.h"
#include "../basic/NativePointer.h"
#include "../basic/SparseVector.h"

#include "../buffer/NativeFloatBuffer.h"
#include "../buffer/NativeDoubleBuffer.h"

#include "retin/toolbox/document/TreeMap.h"

#include "../list/ArrayList.h"
#include "../list/StringList.h"
#include "../list/BytesList.h"
#include "../list/IndexesList.h"
#include "../list/FeatureList.h"
#include "../list/KeypointList.h"
#include "../list/FiltersList.h"

#include "retin/toolbox/document/BytesMatrix.h"
#include "retin/toolbox/document/IndexMatrix.h"
#include "retin/toolbox/document/FloatMatrix.h"
#include "retin/toolbox/document/Matrix.h"
#include "retin/toolbox/document/FeatureMatrix.h"
#include "retin/toolbox/document/Flow2D.h"
#include "retin/toolbox/document/Filter.h"

#include "retin/toolbox/document/SparseMatrix.h"

#include "retin/toolbox/document/GridIntegralImage.h"

#include "retin/toolbox/document/FileName.h"
#include "retin/toolbox/document/ImageFile.h"
#include "retin/toolbox/document/MeshFile.h"
#include "retin/toolbox/document/VideoFile.h"

#include "retin/toolbox/document/GlobalLabel.h"
#include "retin/toolbox/document/BoxLabel.h"
#include "retin/toolbox/document/Labels.h"

#include "retin/toolbox/document/RankingResults.h"
#include "retin/toolbox/document/LogResult.h"
#include "retin/toolbox/document/LogResults.h"

#include "retin/toolbox/document/Parameter.h"
#include "retin/toolbox/document/Parameters.h"

#include "Java.h"

namespace retin {

class StringSerializableCreator : public AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const {
        jstring s = (jstring)obj;
        const char* buf = je.getEnv()->GetStringUTFChars(s,NULL);
        SerializableRef doc = boost::make_shared<String>(buf);
        je.getEnv()->ReleaseStringUTFChars(s,buf);
        return doc;
    }
};

class IntegerSerializableCreator : public AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const {
        return boost::make_shared<Integer>( je.callMethod<jint>(obj,"intValue","()I") );
    }
};

class FloatSerializableCreator : public AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const {
        return boost::make_shared<Float>( je.callMethod<jfloat>(obj,"floatValue","()F") );
    }
};

class DoubleSerializableCreator : public AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const {
        return boost::make_shared<Double>( je.callMethod<jdouble>(obj,"doubleValue","()D") );
    }
};

class ArrayListSerializableCreator : public AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const {
        std::vector<DocumentRef> vect;
        je.createDocument (vect,obj);
        return boost::make_shared<ArrayList>(vect);
    }
};

class TreeMapSerializableCreator : public AbstractSerializableCreator
{
public:
virtual SerializableRef 	createSerializable (JavaEnv& je,jobject obj) const {
        TreeMapRef map = boost::make_shared<TreeMap>();
        je.createDocument (map->getValues(),obj);
        return map;
    }
};

SerializableObjectFactory::SerializableObjectFactory() {

}

SerializableObjectFactory::~SerializableObjectFactory() {
    for (std::map<std::string,AbstractSerializableCreator*>::iterator i=serializableCreators.begin(),e=serializableCreators.end();i!=e;i++)
        if (i->second)
            delete i->second;
}


SerializableObjectFactoryRef defaultSerializableObjectFactoryRef;

SerializableObjectFactoryRef getDefaultSerializableObjectFactory()
{
	if (!defaultSerializableObjectFactoryRef)
	{
		defaultSerializableObjectFactoryRef = boost::make_shared<SerializableObjectFactory>();

		defaultSerializableObjectFactoryRef->registerClass<Integer>(new IntegerSerializableCreator());
		defaultSerializableObjectFactoryRef->registerClass<Float>(new FloatSerializableCreator());
		defaultSerializableObjectFactoryRef->registerClass<Double>(new DoubleSerializableCreator());
		defaultSerializableObjectFactoryRef->registerClass<String>(new StringSerializableCreator());

		defaultSerializableObjectFactoryRef->registerClass<ArrayList>(new ArrayListSerializableCreator());
		defaultSerializableObjectFactoryRef->registerClass<TreeMap>(new TreeMapSerializableCreator());

		defaultSerializableObjectFactoryRef->registerClass<Text>();
		defaultSerializableObjectFactoryRef->registerClass<Indexes>();
		defaultSerializableObjectFactoryRef->registerClass<Feature>();
		defaultSerializableObjectFactoryRef->registerClass<Keypoint>();
                
                defaultSerializableObjectFactoryRef->registerClass<SparseVector>();
                defaultSerializableObjectFactoryRef->registerClass<SparseMatrix>();
                

                defaultSerializableObjectFactoryRef->registerClass<NativeFloatBuffer>();
                defaultSerializableObjectFactoryRef->registerClass<NativeDoubleBuffer>();
                defaultSerializableObjectFactoryRef->registerClass<NativePointer>();
                

		defaultSerializableObjectFactoryRef->registerClass<FileName>();
		defaultSerializableObjectFactoryRef->registerClass<ImageFile>();
		defaultSerializableObjectFactoryRef->registerClass<MeshFile>();
		defaultSerializableObjectFactoryRef->registerClass<VideoFile>();
 
		defaultSerializableObjectFactoryRef->registerClass<StringList>();
		defaultSerializableObjectFactoryRef->registerClass<BytesList>();
		defaultSerializableObjectFactoryRef->registerClass<IndexesList>();
		defaultSerializableObjectFactoryRef->registerClass<FeatureList>();
		defaultSerializableObjectFactoryRef->registerClass<KeypointList>();
                defaultSerializableObjectFactoryRef->registerClass<FiltersList>();
                
		defaultSerializableObjectFactoryRef->registerClass<Matrix>();
		defaultSerializableObjectFactoryRef->registerClass<FloatMatrix>();
		defaultSerializableObjectFactoryRef->registerClass<BytesMatrix>();
		defaultSerializableObjectFactoryRef->registerClass<IndexMatrix>();
		defaultSerializableObjectFactoryRef->registerClass<FeatureMatrix>();
                defaultSerializableObjectFactoryRef->registerClass<Flow2D>();
                defaultSerializableObjectFactoryRef->registerClass<Filter>();
                
		defaultSerializableObjectFactoryRef->registerClass<GridIntegralImage>();

		defaultSerializableObjectFactoryRef->registerClass<GlobalLabel>();
		defaultSerializableObjectFactoryRef->registerClass<BoxLabel>();
		defaultSerializableObjectFactoryRef->registerClass<Labels>();

		defaultSerializableObjectFactoryRef->registerClass<RankingResults>();
		defaultSerializableObjectFactoryRef->registerClass<LogResult>();
		defaultSerializableObjectFactoryRef->registerClass<LogResults>();

		defaultSerializableObjectFactoryRef->registerClass<Parameter>();
		defaultSerializableObjectFactoryRef->registerClass<Parameters>();
	}
	return defaultSerializableObjectFactoryRef;
}

}
