/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.toolbox.nativegenerator;

import java.lang.reflect.Constructor;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.TreeMap;
import retin.toolbox.database.Database;
import retin.toolbox.database.Manager;
import retin.toolbox.database.Table;
import retin.toolbox.database.generator.GeneratorDef;
import retin.toolbox.document.Options;
import retin.toolbox.document.Parameters;

/**
 *
 * @author romanegr
 */
public final class NativeDocumentGeneratorRegister {
    private static volatile NativeDocumentGeneratorRegister instance = null;
    
    private AbstractMap<String, Class> nativeDocumentGenerator;

    private NativeDocumentGeneratorRegister() {
        super();
        nativeDocumentGenerator = new TreeMap<String, Class> ();
    }

    public static NativeDocumentGeneratorRegister getInstance() {

        if (NativeDocumentGeneratorRegister.instance == null) {
            synchronized(NativeDocumentGeneratorRegister.class) {
                if (NativeDocumentGeneratorRegister.instance == null)
                    NativeDocumentGeneratorRegister.instance = new NativeDocumentGeneratorRegister();
            }
        }

        return NativeDocumentGeneratorRegister.instance;
    }
    
    public static void getStringSelectionOfNativeGenerator(Class generatorType, String [] inputTypes, Options op, Manager manager, AbstractMap<String, String> args) {
        if(NativeDocumentGeneratorRegister.instance == null)
            return;
        
        try {
            NativeDocumentGeneratorRegister ndgr = NativeDocumentGeneratorRegister.instance;
            Options.StringSelection ss = op.addStringSelection("featuretype");
            String featureName[];
            
            featureName = ndgr.getGeneratorsNames(NativeFeatureListGenerator.class, inputTypes);
            for(int i = 0 ; i < featureName.length ; i++)
                ss.add(featureName[i], ndgr.getOptionsOf(featureName[i], manager, args));
            
        } catch (Exception ex) {
            System.out.println(ex);
        }
        
    }
    
    public static NativeDocumentGenerator getInitialisedGenerator(Manager manager, String name, String base, String table, Parameters params){
        if(NativeDocumentGeneratorRegister.instance == null)
            return null;
        return getInitialisedGenerator(name, manager.getTable(base, table), params);
    }
    
    public static NativeDocumentGenerator getInitialisedGenerator(String name, Table table, Parameters params){
        if(NativeDocumentGeneratorRegister.instance == null)
            return null;
        NativeDocumentGenerator natGen = null;
        try {
            natGen = NativeDocumentGeneratorRegister.getInstance().getNativeDocumentGenerator(name);
            natGen.initParentTable(table);
            params.set("name", name);
            params.set("description",NativeDocumentGeneratorRegister.getInstance().getDescriptionOf(name,params,false));
            natGen.initParams(params);
        } catch (Exception ex) {
            System.out.println(NativeDocumentGeneratorRegister.class.getName() + " " + ex.getMessage());
            natGen = null;
        }
        
        return natGen;
    }
    
    public void registerNativeDocumentGenerator(Class ndgc) throws Exception {
        if(!NativeDocumentGenerator.class.isAssignableFrom(ndgc))
            throw new Exception(ndgc.getName() + " is not NativeDocumentGenerator");      
        
        String name = getNativeDocumentGenerator(ndgc).getName();
        
        System.out.println("register native generator : " + name);
        
        nativeDocumentGenerator.put(name, ndgc);
    }
    
    public String[] getGeneratorsNames(Class generatorType, String [] inputTypes) throws Exception {
        ArrayList<String> names = new ArrayList<String>();
        
        for (String mapKey : nativeDocumentGenerator.keySet()) {
            Class ndgc = nativeDocumentGenerator.get(mapKey);
            if(generatorType.isAssignableFrom(ndgc)){
                String ndgInputTypes [] = getNativeDocumentGenerator(ndgc).getSupportedInputType();
                for(String ndgInputType : ndgInputTypes) {
                    for(String inputType : inputTypes) {
                        if(ndgInputType.equals(inputType)) {
                            if(!names.contains(mapKey))
                                names.add(mapKey);
                        }
                    }
                }
            }
        }
        
        Object [] objectArray = names.toArray();    
        return Arrays.copyOf(objectArray, objectArray.length, String[].class) ;
    }
    
    public Class getClassOf(String name) throws Exception {
        return nativeDocumentGenerator.get(name);
    }
    
    public Constructor getContructorOf(String name) throws Exception {
        return getClassOf(name).getConstructor(new Class[] {});
    }
    
    public NativeDocumentGenerator getNativeDocumentGenerator(String name) throws Exception {
        return (NativeDocumentGenerator) getContructorOf(name).newInstance();
    }
    
    public int getDimOf(String name, Parameters params) throws Exception {
        if(params.get("pca", "no").equals("yes")){
            return params.getParams("pca").getAsInt("dim");
        }
        return getNativeDocumentGenerator(name).getDim(params);
    }
    
    static public String[] getListDocuments(Database database){
        return getListDocuments(database, "documents/");
    }
    
    static public String[] getListDocuments(Database database, String dir){
        return database.getTablePathContent(dir,true);
    }
    
    public Options getOptionsOf(String name, Manager manager, AbstractMap<String, String> args) throws Exception {
        Options o;
        o = getNativeDocumentGenerator(name).getOptions(manager, args);
        
        String baseURL = args.get("baseURL");
        
        Database database = manager.getDatabase(baseURL);
        
        Options oPca = new Options();
        oPca.addString("baseURL").setDefault(baseURL);
        oPca.addStringSelection("document").add(getListDocuments(manager.getDatabase(baseURL)));
        oPca.addString("train").setDefault(database.guessTableName(null,"train"));
        oPca.addInteger("maxdoc").setRange(0, 100000).setDefault(10000);
        oPca.addInteger("maxfeatperdoc").setRange(0, 100000).setDefault(2000);
        oPca.addDouble("pcfeatperdoc").setRange(1E-3, 1).setDefault(0.1);
        oPca.addInteger("dim").setRange(0, 100000).setDefault(80);
        oPca.addStringSelection("whitening").add("yes").add("no").setDefault("no");
        
        Options oPower = new Options();
        oPower.addDouble("power").setRange(1E-6, 1).setDefault(0.5);
        
        o.addStringSelection("power").add("yes",oPower).add("no").setDefault("no");
        o.addStringSelection("norm").add("l2").add("none").setDefault("none");
        o.addStringSelection("pca").add("yes",oPca).add("no").setDefault("no");
        return o;
    }
    
    public String getDescriptionOf(String name, Parameters params) throws Exception {
        return getDescriptionOf(name, params, true);
    }
    
    public String getDescriptionOf(String name, Parameters params, boolean withPca) throws Exception {
        String d;
        d = getNativeDocumentGenerator(name).getDescription(params);
        if(params.get("power", "no").equals("yes")){
            d += "-p" + params.getParams("power").get("power");
        }
        if(params.get("norm", "none").equals("l2")){
            d += "-l2";
        }
        if(withPca && params.get("pca", "no").equals("yes")){
            d += "-pca" + params.getParams("pca").get("dim");
            if(params.getParams("pca").get("whitening").equals("yes"))
                d+="w";
        }
        return d;
    }
    
    public void creatpreprocessingTable(Manager manager, String urlBase, String name, Parameters params) throws Exception{
        String tableName = "nativenenerator";
        tableName += "/"+ name;
        
        String key = getDescriptionOf(name, params, false);
        
        if(params.get("pca", "no").equals("no"))
             return;
        
        if(params.get("pca", "no").equals("yes")){
            Table paramsTable = manager.addTable(urlBase, tableName + "/params", "Parameters", "xml");
            manager.addTable(urlBase, tableName + "/mean", "FloatMatrix", "xml");
            manager.addTable(urlBase, tableName + "/cov", "FloatMatrix", "xml");
            manager.addTable(urlBase, tableName + "/value", "FloatMatrix", "xml");
            Table vectorTable = manager.addTable(urlBase, tableName + "/vector", "FloatMatrix", "xml");
            
            vectorTable.setDocumentGenerator(new GeneratorDef()
                    .setProcess(CompEig.class)
                    .setDocumentInput("input", urlBase, tableName + "/params", null)
                    .setParam("name", name)
                    .setDocumentOutput("meantable", urlBase, tableName + "/mean", null)
                    .setDocumentOutput("covtable", urlBase, tableName + "/cov", null)
                    .setDocumentOutput("valuetable", urlBase, tableName + "/value", null));
            
            paramsTable.setDocument(key, params);
        }
    }
    
    private NativeDocumentGenerator getNativeDocumentGenerator(Class ndgc) throws Exception {
        return (NativeDocumentGenerator) ndgc.newInstance();
    }
}
