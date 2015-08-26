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
#include "NativeFeatureListGenerator.h"
#include "retin/toolbox/database/LocalDatabase.h"
#include "retin/toolbox/algebra/matrix_float.h"

using namespace std;
using namespace boost;
using namespace retin;

void NativeFeatureListGenerator::initParams(ParametersRef params){
    if(params->get("pca","no") == "yes"){
        pcaEn = true;
        //dim = params->getSubParams("pca")->getAs<unsigned int>("dim");
        dim = params->getSubParams("pca")->getAs<size_t>("dim");
	if(params->getSubParams("pca")->get("whitening","no") == "yes"){
            whitening = true;
        }
        else {
            whitening = false;
        }
        
        LocalDatabase ldb(params->getSubParams("pca")->get("baseURL"));
        TableRef eigVatTable;
        TableRef eigMeanTable;
        TableRef eigVectTable;
        
        string dir = "nativenenerator/"+params->get("name");
        
        eigVatTable = ldb.getTable(dir+"/value");
        eigVectTable = ldb.getTable(dir+"/vector");
        eigMeanTable = ldb.getTable(dir+"/mean");
        
        mean = eigMeanTable->getDocumentAs<FloatMatrix>(params->get("description"));
        eigValues = eigVatTable->getDocumentAs<FloatMatrix>(params->get("description"));
        eigVectors = eigVectTable->getDocumentAs<FloatMatrix>(params->get("description"));
        invsqrteigValues = make_shared<FloatMatrix>(eigValues->vectorSize(),eigValues->vectorCount());
        float * t1 = eigValues->data();
        float * t2 = invsqrteigValues->data();
        for(size_t i = 0 ; i < eigValues->vectorSize() ; i ++){
            t2[i] = 1.0f/sqrt(t1[1]);
        }
    }
    else
        pcaEn = false;

    if(params->get("power","no") == "yes"){
        powerEn = true;
        power = params->getSubParams("power")->getAs<float>("power");
    }
    else
        powerEn = false;

    if(params->get("norm","none") == "l2")
        l2En = true;
    else
        l2En = false;
    
}

DocumentRef NativeFeatureListGenerator::computeFile(std::string addressFile){
    FeatureListRef f = computeFeatureListFile(addressFile);
    if(powerEn && l2En)
        f->powerNorm(power, "l2");
    else if(powerEn && !l2En)
        f->powerNorm(power, "none");
    else if(!powerEn && l2En)
        f->powerNorm(1.0f, "l2");

    
    if(pcaEn){
        FeatureListRef f2 = make_shared<FeatureList > (dim, f->size());
        for(size_t i = 0 ; i < (size_t)f->size() ; i++){            
            vector_linear_float(f->data(i), f->data(i), -1, mean->data(), f->dim());
            
            memset(f2->data(i),0,sizeof(float)*dim);
            matrix_CpAtB_float(f2->data(i), eigVectors->data(), f->data(i), dim, f->dim(), 1);

            if(whitening)
                vector_mul_float (f2->data(i), invsqrteigValues->data(), dim);
        }
        f = f2;
    }
    return f;
}
