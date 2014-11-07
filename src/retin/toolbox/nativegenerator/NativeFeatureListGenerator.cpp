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
