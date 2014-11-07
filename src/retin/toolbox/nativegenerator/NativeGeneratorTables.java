/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package retin.toolbox.nativegenerator;

import java.io.PrintWriter;
import java.util.AbstractMap;
import retin.toolbox.database.Database;
import retin.toolbox.database.FeaturesManager;
import retin.toolbox.database.Manager;
import retin.toolbox.database.Table;
import retin.toolbox.database.featuredef.BagFeatureDef;
import retin.toolbox.database.generator.GeneratorDef;
import retin.toolbox.database.processing.ManagerProcess;
import retin.toolbox.database.processing.ManagerProcessContext;
import retin.toolbox.database.processing.Operation;
import retin.toolbox.document.Document;
import retin.toolbox.document.Options;
import retin.toolbox.document.Parameters;


/**
 *
 * @author romanegr
 */
public class NativeGeneratorTables extends Operation {
    
    NativeGeneratorTables(){
        super("NativeGeneratorTables", "Features/Native Generator Tables");
    }
    
    @Override
    public boolean isAtomic() {
        return true;
    }

    @Override
    public boolean databaseListChanged() {
        return false;
    }

    @Override
    public boolean tableListChanged() {
        return true;
    }

    @Override
    public Options getOptions(Manager manager, AbstractMap<String, String> args) {
        String baseURL = args.get("baseURL");
        Database database = manager.getDatabase(baseURL);
        Options options = new Options();
        options.addString("baseURL").setDefault(baseURL);
        options.addMultipleStringSelection("documents").add(NativeDocumentGeneratorRegister.getListDocuments(database));
        NativeDocumentGeneratorRegister.getStringSelectionOfNativeGenerator(
            NativeFeatureListGenerator.class, 
            new String []{NativeDocumentGenerator.VIDEOTYPE, NativeDocumentGenerator.IMAGETYPE, NativeDocumentGenerator.FILE3DTYPE},
            options, manager, args);
        return options;
    }

    public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
        String baseURL = params.get("baseURL");
        Database database = manager.getDatabase(baseURL);
        FeaturesManager fm = database.getFeaturesManager();
        String[] documents =   params.getParams("documents")
                                        .getNames();
        
        String featuretype = params.get("featuretype");
        Parameters featureParams = params.getParams("featuretype");
        
        int dim = NativeDocumentGeneratorRegister.getInstance().getDimOf(featuretype, featureParams);
        
        NativeDocumentGeneratorRegister.getInstance().creatpreprocessingTable(manager, baseURL, featuretype, featureParams);
        
        for(int d = 0 ; d < documents.length ; d++){
            String document = documents[d];
            String fullName = featuretype + "-" + document.replace("documents/", "").replace("/", "_");
            fullName += "-" + NativeDocumentGeneratorRegister.getInstance().getDescriptionOf(featuretype, featureParams);
            String tableName = "features/" + featuretype + "/";
            tableName += document.replace("documents/", "").replace("/", "_");
            tableName += "-" + NativeDocumentGeneratorRegister.getInstance().getDescriptionOf(featuretype, featureParams);
            
            
            
            Table featuresTable = manager.addTable(baseURL, tableName, "FeatureList", "bin");
            
            featuresTable.setDocumentGenerator(new GeneratorDef()
                    .setContextProcess(CompFeaturesNativeGeneratorContext.class)
                    .setParam("featuretype", params.get("featuretype"), params.getParams("featuretype"))
                    .setDocumentInput("input", baseURL, document, null));
                                    
            
            fm.registerFeature (fullName+"-sift",new BagFeatureDef(document)
                        .setBagTable(baseURL, tableName, dim));
            manager.save();
        }
    }
    
    public static class CompFeaturesNativeGeneratorContext implements ManagerProcessContext {

        NativeDocumentGenerator ngd;
        
        public ManagerProcess createProcess() {
            return new CompFeaturesNativeGenerator(ngd);
        }

        public void dispose() {
        }

        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            ngd =  NativeDocumentGeneratorRegister.
                    getInitialisedGenerator(manager, 
                                            params.get("featuretype"), 
                                            params.getParams("input").get("base"),
                                            params.getParams("input").get("table"),
                                            params.getParams("featuretype"));
        }
    }
    
    public static class CompFeaturesNativeGenerator implements ManagerProcess {

        NativeDocumentGenerator ngd;
        
        CompFeaturesNativeGenerator(NativeDocumentGenerator _ngd){
            ngd = _ngd;
        }
        
        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
             Document output = ngd.computeDocument(params.getParams("input").get("doc"));
             manager.setDocument(params.getParams("output"), output);
        }
    
    }
}
