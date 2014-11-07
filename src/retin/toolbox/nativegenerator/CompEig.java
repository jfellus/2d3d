
package retin.toolbox.nativegenerator;

import java.io.PrintWriter;
import retin.toolbox.database.Manager;
import retin.toolbox.database.Table;
import retin.toolbox.database.processing.ManagerProcess;
import retin.toolbox.document.FloatMatrix;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.basic.NativePointer;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author romanegr
 */
public class CompEig implements ManagerProcess{
    
    int maxDoc;
    int maxFeatPerDoc;
    float pcFeatPerDoc;
    
    int dimFeat;
    
    NativeFeatureListGenerator natGen;
    
    FloatMatrix cov;
    FloatMatrix mean;
    FloatMatrix eigValues;
    FloatMatrix eigVectors;
    
    public boolean isAtomic() {
        return false;
    }

    public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
        Parameters eigParams = (Parameters) manager.getDocumentAs(Parameters.class, params.getParams("input"));    
        
        maxDoc = eigParams.getParams("pca").getAsInt("maxdoc");
        maxFeatPerDoc = eigParams.getParams("pca").getAsInt("maxfeatperdoc");
        pcFeatPerDoc = (float) eigParams.getParams("pca").getAsDouble("pcfeatperdoc");
        
        Table trainTable = manager.getTable(eigParams.getParams("pca").get("baseURL"),eigParams.getParams("pca").get("train"));
        String[] trainKeys = trainTable.getRandomKeys(maxDoc);
        
        natGen = (NativeFeatureListGenerator) NativeDocumentGeneratorRegister.getInstance().getNativeDocumentGenerator(params.get("name"));
        natGen.initParentTable(manager, 
                    eigParams.getParams("pca").get("baseURL"), 
                    eigParams.getParams("pca").get("document"));

        eigParams.remove("pca");

        natGen.initParams(eigParams);
        
        dimFeat = natGen.getDim(eigParams);
        
        compCovMatrix(natGen.getInstance(),trainKeys);

        compEigMatrices();
        
        manager.setDocument(params.getParams("meantable"),  mean);
        manager.setDocument(params.getParams("covtable"),  cov);
        manager.setDocument(params.getParams("valuetable"),  eigValues);
        manager.setDocument(params.getParams("output"),  eigVectors);
    }
    
    native void compCovMatrix(NativePointer instance, String[] keys);
    native void compEigMatrices();
    
}
