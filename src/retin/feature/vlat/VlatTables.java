package retin.feature.vlat;

import java.io.PrintWriter;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.List;
import retin.toolbox.database.Database;
import retin.toolbox.database.FeaturesManager;
import retin.toolbox.database.Manager;
import retin.toolbox.database.Table;
import retin.toolbox.database.featuredef.FeatureDef;
import retin.toolbox.database.featuredef.GlobalFeatureDef;
import retin.toolbox.database.generator.GeneratorDef;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.database.processing.ManagerProcess;
import retin.toolbox.database.processing.ManagerProcessContext;
import retin.toolbox.database.processing.Operation;
import retin.toolbox.document.FloatMatrix;
import retin.toolbox.document.Matrix;
import retin.toolbox.document.Options;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.list.FeatureList;
import retin.toolbox.nativegenerator.NativeDocumentGeneratorRegister;
import retin.toolbox.nativegenerator.NativeFeatureListGenerator;
import retin.toolbox.quantization.ElbgQuantizer;


public class VlatTables extends Operation {

    public VlatTables() {
        super("VlatTables","Features/Vlat Tables");
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
        fm = database.getFeaturesManager();

        String[] bagFeatures = fm.getFeatureNames("bag");
        
        String[] imageTables = database.getTablePathContent("documents/",true);

        Options featurePca = new Options();
        featurePca.addStringSelection("strategy").add("maxEnergy").add("maxDims").add("most").add("least").setDefault("maxEnergy");
        featurePca.addDouble("maxEnergy").setRange(0.01,100).setDefault(80);
        featurePca.addInteger("maxDims").setRange(0,1000000000).setDefault(128);
        featurePca.addStringSelection("whitening").add("yes").add("no").setDefault("no");
        
        Options elbg = new Options();
        elbg.addInteger("maxIterations").setRange(1, 1000).setDefault(100);
        elbg.addStringValues("maxWords").addDefault("64").addDefault("256").addDefault("1024");
        elbg.addInteger("maxDocs").setRange(1, 1000000).setDefault(10000);
        elbg.addDouble("perFeaturesPerDoc").setRange(0.01,1).setDefault(1);
        elbg.addInteger("maxFeaturesPerDoc").setRange(0,1000000000).setDefault(0);
        elbg.addInteger("verboseLevel").setRange(0,100).setDefault(0);
        elbg.addStringSelection("pca").add("yes",featurePca).add("no").setDefault("no");
        
        Options projected = new Options();
        projected.addStringSelection("centered").add("no").add("yes").setDefault("no");
        
        Options standard = new Options();
        standard.addStringSelection("format").add("full").add("L").add("LD").setDefault("LD");
        standard.addStringSelection("pooling").add("sum").add("ewp").add("ewn").add("cw").setDefault("sum");
        
        Options sparse = new Options();
        sparse.addStringSelection("format").add("full").add("L").add("LD").setDefault("LD");
        sparse.addStringSelection("on").add("diag").add("full").add("value").setDefault("diag");
        sparse.addStringSelection("sparse").add("dim").add("energy").setDefault("dim");
        sparse.addInteger("dim").setRange(1, 100000).setDefault(128);
        sparse.addDouble("energy").setRange(1E-6f, 1.0f).setDefault(1.0f);

        Options packed = new Options();
        packed.addStringValues("MSE").addDefault("0").addDefault("0.1").addDefault("0.2").addDefault("0.3");

        Options pca = new Options();
        pca.addInteger("maxSamples").setRange(1, 1000000).setDefault(2000);
        pca.addInteger("maxEnergy").setDefault(100);
        pca.addStringValues("maxDims").addDefault("16").addDefault("32").addDefault("64").addDefault("128");
        pca.addStringSelection("format").add("full").add("L").add("LD").setDefault("L");

        Options inputFeature = new Options();
        inputFeature.addMultipleStringSelection("features").add(bagFeatures);
                
        Options inputImage = new Options();
        inputImage.addMultipleStringSelection("images").add(imageTables);
        NativeDocumentGeneratorRegister.getStringSelectionOfNativeGenerator(
            NativeFeatureListGenerator.class, 
            new String []{NativeDocumentGenerator.VIDEOTYPE, NativeDocumentGenerator.IMAGETYPE, NativeDocumentGenerator.FILE3DTYPE},
            inputImage, manager, args);
        
        Options vlat = new Options();

        vlat.addString("baseURL").setDefault(baseURL);
        vlat.addStringSelection("input").add("feature",inputFeature).add("image",inputImage).setDefault("feature");
        vlat.addInteger("degre").setRange(0, 10).setDefault(2);
        vlat.addString("trainBase").setDefault(baseURL);
        vlat.addString("trainTable").setDefault(database.guessTableName(null,"train"));
        vlat.addStringSelection("dict").add("elbg",elbg);
        vlat.addStringSelection("standard").add("yes",standard).add("no").setDefault("yes");
        vlat.addStringSelection("sparse").add("yes",sparse).add("no").setDefault("no");
        vlat.addStringSelection("projected").add("yes").add("no").setDefault("no");
        vlat.addStringSelection("packed").add("yes",packed).add("no").setDefault("no");
        vlat.addStringSelection("pca").add("yes",pca).add("no").setDefault("no");
        
        return vlat;
         
    }

    void sparseTables(String featurePreName,String tablePreName,int t, Parameters methodParams, int o, int s, int d, float e) throws Exception {
        String featureName = featurePreName + "-" + t + "-s";
        String histTable = tablePreName + "-" + t + "-s";
        
        if(s == SparseVlatBuilder.sparsityStrategyDim){
            featureName += "d" + d;
            histTable += "d" + d;
        }
        else if(s == SparseVlatBuilder.sparsityStrategyEnergy){
            featureName += "e" + e;
            histTable += "e" + e;
        }
        
        if(o == SparseVlatBuilder.sparsityOnFull){
            featureName += "f";
            histTable += "f";
        }
        else if(o == SparseVlatBuilder.sparsityMaxValues){
            featureName += "mv";
            histTable += "mv";
        }

        String format = methodParams.get("format");
        if (!format.equals("full")) {
            featureName += format;
            histTable += format;
        }

        Table table = manager.addTable(parentBagBase, histTable, "Feature", "bin");
        table.setDocumentGenerator(new GeneratorDef()
                .setContextProcess(CompSparseVlatContext.class)
                .setParam("degre",degre)
                .setParam("format",format)
                .setParam("on",o)
                .setParam("strategy",s)
                .setParam("sparsedim",d)
                .setParam("sparseenergy",e)
                .setDocumentInput("input",parentBagBase,parentBagTable,null)
                .setDocumentInput("dict",parentBagBase,dictTable,String.valueOf(maxWords))
                .setDocumentInput("tensors",parentBagBase,tensorTable,String.valueOf(maxWords))
        );
        int dim = SparseVlatBuilder.getDim(maxWords, t, featureDim, format);
        fm.registerFeature(featureName, new GlobalFeatureDef().setHistTable(parentBagBase, histTable, dim));
    }
    
    void sparseTablesImages(String featurePreName,String tablePreName,int t, Parameters methodParams, String featuretype, Parameters featureParams, int o, int s, int d, float e) throws Exception {
        String featureName = featurePreName + "-" + t + "-s";
        String histTable = tablePreName + "-" + t + "-s";

        if(s == SparseVlatBuilder.sparsityStrategyDim){
            featureName += "d" + d;
            histTable += "d" + d;
        }
        else if(s == SparseVlatBuilder.sparsityStrategyEnergy){
            featureName += "e" + e;
            histTable += "e" + e;
        }
        
        if(o == SparseVlatBuilder.sparsityOnFull){
            featureName += "f";
            histTable += "f";
        }
        else if(o == SparseVlatBuilder.sparsityMaxValues){
            featureName += "mv";
            histTable += "mv";
        }
        
        String format = methodParams.get("format");
        if (!format.equals("full")) {
            featureName += format;
            histTable += format;
        }

        Table table = manager.addTable(parentBase, histTable, "Feature", "bin");
        table.setDocumentGenerator(new GeneratorDef()
                .setContextProcess(CompSparseVlatContext.class)
                .setParam("degre",degre)
                .setParam("format",format)
                .setParam("on",o)
                .setParam("strategy",s)
                .setParam("sparsedim",d)
                .setParam("sparseenergy",e)
                .setParam("featuretype", featuretype, featureParams)
                .setDocumentInput("input",parentBase,parentTable,null)
                .setDocumentInput("dict",parentBase,dictTable,String.valueOf(maxWords))
                .setDocumentInput("tensors",parentBase,tensorTable,String.valueOf(maxWords))
        );
        int dim = SparseVlatBuilder.getDim(maxWords, t, featureDim, format);
        fm.registerFeature(featureName, new GlobalFeatureDef().setHistTable(parentBase, histTable, dim));
    }
    
    void standardTables(String featurePreName,String tablePreName,int t, Parameters methodParams) throws Exception {
        String featureName = featurePreName + "-" + t;
        String histTable = tablePreName + "-" + t;

        String format = methodParams.get("format");
        if (!format.equals("full")) {
            featureName += format;
            histTable += format;
        }
        String pooling = methodParams.get("pooling");
        if (!pooling.equals("sum")) {
            featureName += pooling;
            histTable += pooling;
        }

        Table table = manager.addTable(parentBagBase, histTable, "Feature", "bin");
        table.setDocumentGenerator(new GeneratorDef()
                .setContextProcess(CompVlatContext.class)
                .setParam("degre",degre)
                .setParam("format",format)
                .setParam("pooling",pooling)
                .setDocumentInput("input",parentBagBase,parentBagTable,null)
                .setDocumentInput("dict",parentBagBase,dictTable,String.valueOf(maxWords))
                .setDocumentInput("tensors",parentBagBase,tensorTable,String.valueOf(maxWords))
        );
        int dim = StandardVlatBuilder.getDim(maxWords, t, featureDim, format);
        fm.registerFeature(featureName, new GlobalFeatureDef().setHistTable(parentBagBase, histTable, dim));
    }
   
    void standardTablesImage(String featurePreName, String tablePreName, int t, Parameters methodParams, String featuretype, Parameters featureParams) throws Exception {
        String featureName = featurePreName + "-" + t;
        String histTable = tablePreName + "-" + t;

        String format = methodParams.get("format");
        if (!format.equals("full")) {
            featureName += format;
            histTable += format;
        }
        String pooling = methodParams.get("pooling");
        if (!pooling.equals("sum")) {
            featureName += pooling;
            histTable += pooling;
        }
        
        Table table = manager.addTable(parentBase, histTable, "Feature", "bin");
        table.setDocumentGenerator(new GeneratorDef()
                .setContextProcess(CompVlatContext.class)
                .setParam("degre",degre)
                .setParam("format",format)
                .setParam("pooling",pooling)
                .setParam("featuretype", featuretype, featureParams)
                .setDocumentInput("input",parentBase,parentTable,null)
                .setDocumentInput("dict",parentBase,dictTable,String.valueOf(maxWords))
                .setDocumentInput("tensors",parentBase,tensorTable,String.valueOf(maxWords))
        );
        int dim = StandardVlatBuilder.getDim(maxWords, t, featureDim, format);
        fm.registerFeature(featureName, new GlobalFeatureDef().setHistTable(parentBase, histTable, dim));
    }
 
    void projectedTablesImage(String featurePreName, String tablePreName, String featuretype, Parameters featureParams) throws Exception {
        String featureName = featurePreName + "-projected";
        String histTable = tablePreName + "-projected";
        
        Table table = manager.addTable(parentBase, histTable, "Feature", "bin");
        table.setDocumentGenerator(new GeneratorDef()
                .setContextProcess(CompProjectedVlatContext.class)
                .setParam("degre",degre)
                .setParam("featuretype", featuretype, featureParams)
                .setDocumentInput("input",parentBase,parentTable,null)
                .setDocumentInput("dict",parentBase,dictTable,String.valueOf(maxWords))
                .setDocumentInput("tensors",parentBase,tensorTable,String.valueOf(maxWords))
        );
        int dim = ProjectedVlatBuilder.getDim(maxWords, featureDim);
        fm.registerFeature(featureName, new GlobalFeatureDef().setHistTable(parentBase, histTable, dim));
    }

    void packedTables(String featurePreName,String tablePreName,int t,Parameters methodParams) throws Exception {

        if (t != 2)
            throw new IllegalArgumentException("Only order 2 is supported");

        String[] mses = methodParams.getParams("MSE").getNames();
        for (int m=0;m<mses.length;m++) {
            double mse = Double.parseDouble(mses[m]);

            String featureName = featurePreName + "-" + t + "-" + mse;
            String histTable = tablePreName + "-" + t + "-" + mse;

            Table table = manager.addTable(parentBagBase, histTable, "PackedVLAT", "bin");
            table.setDocumentGenerator(new GeneratorDef()
                    .setContextProcess(CompPackedVlatContext.class)
                    .setParam("degre",degre)
                    .setParam("mse",mse)
                    .setDocumentInput("input",parentBagBase,parentBagTable,null)
                    .setDocumentInput("dict",parentBagBase,dictTable,String.valueOf(maxWords))
                    .setDocumentInput("tensors",parentBagBase,tensorTable,String.valueOf(maxWords))
            );

            fm = manager.getFeaturesManager(parentBagBase);
            fm.registerFeature(featureName, new FeatureDef("packedvlat")
                    .setTable("data",parentBagBase, histTable)
                    .setDocument("dict", parentBagBase, dictTable, String.valueOf(maxWords))
                    .setDocument("tensors", parentBagBase, tensorTable, String.valueOf(maxWords))
            );
        }
    }

    void pcaTables(String featurePreName,String tablePreName,int t,Parameters methodParams) throws Exception
    {
        String[] pcaDims = methodParams.getParams("maxDims").getNames();
        int pcaEnergy = methodParams.getAsInt("maxEnergy");
        int maxSamples = methodParams.getAsInt("maxSamples");

        String pcasizeTable = tablePreName + "-pca/sizes";
        String pcadictTable = tablePreName + "-pca/dicts";
        String pcameanTable = tablePreName + "-pca/means";
        String pcavarTable = tablePreName + "-pca/vars";
        String pcatensorTable = tablePreName + "-pca/tensors";

        Table sizetable = manager.addTable(parentBagBase, pcasizeTable, "Parameters");

        manager.addTable(parentBagBase, pcameanTable, "ArrayList", "bin");
        manager.addTable(parentBagBase, pcavarTable, "ArrayList", "bin");
        manager.addTable(parentBagBase, pcadictTable, "ArrayList", "bin")
                .setDocumentGenerator(new GeneratorDef()
                        .setProcess(CompVlatPcaDict.class)
                        .setParam("degre",degre)
                        .setDocumentInput("input",parentBagBase,pcasizeTable,null)
                        .setDocumentInput("dict",parentBagBase,dictTable,String.valueOf(maxWords))
                        .setTableInput("bags",parentBagBase,parentBagTable)
                        .setTableInput("train", mainTrainBase, mainTrainTable)
                        .setDocumentOutput("pcameans",parentBagBase,pcameanTable,null)
                        .setDocumentOutput("pcavars",parentBagBase,pcavarTable,null)
                );

        String format = methodParams.get("format");
        manager.addTable(parentBagBase, pcatensorTable, "ArrayList", "bin")
                .setDocumentGenerator(new GeneratorDef()
                        .setProcess(CompVlatPcaTensor.class)
                        .setParam("degre",degre)
                        .setParam("format",format)
                        .setDocumentInput("dict",parentBagBase,dictTable,String.valueOf(maxWords))
                        .setTableInput("bags",parentBagBase,parentBagTable)
                        .setTableInput("train", mainTrainBase, mainTrainTable)
                        .setDocumentInput("pcadicts",parentBagBase,pcadictTable,null)
                        .setDocumentInput("pcameans",parentBagBase,pcameanTable,null)
                        .setDocumentInput("pcavars",parentBagBase,pcavarTable,null)
                );

        for (int s = 0; s < pcaDims.length; s++) {
            int maxDim = Integer.parseInt(pcaDims[s]);
            String pcaName = maxDim + "-" + pcaEnergy;
            sizetable.setDocument(pcaName, new Parameters("maxSamples=" + maxSamples + ",maxDim=" + maxDim + ",maxEnergy=" + pcaEnergy));


            String featureName = parentName + "-vlat" + maxWords + "-pca" + pcaName + "-" + t;
            String histTable = parentBagTable + "-vlat/" + maxWords + "-pca/" + pcaName + "-" + t;

            if (t > 1 && !format.equals("full")) {
                featureName += format;
                histTable += format;
            }

            manager.addTable(parentBagBase, histTable, "Feature", "bin")
                .setDocumentGenerator(new GeneratorDef()
                        .setContextProcess(CompVlatPcaContext.class)
                        .setParam("degre",degre)
                        .setParam("format",format)
                        .setDocumentInput("input",parentBagBase,parentBagTable,null)
                        .setDocumentInput("dict",parentBagBase,dictTable,String.valueOf(maxWords))
                        .setDocumentInput("pcadicts",parentBagBase,pcadictTable,pcaName)
                        .setDocumentInput("pcameans",parentBagBase,pcameanTable,pcaName)
                        .setDocumentInput("pcavars",parentBagBase,pcavarTable,pcaName)
                );

            fm = manager.getFeaturesManager(parentBagBase);
            fm.registerFeature(featureName, new GlobalFeatureDef().setHistTable(parentBagBase, histTable, StandardVlatBuilder.getDim(maxWords,t,maxDim,format)));
        }

    }

    FeaturesManager fm;
    Manager manager;
    int degre,featureDim,maxWords;
    String mainTrainBase, mainTrainTable;
    String parentName,parentBagBase,parentBagTable;
    String parentDocumentName, parentBase, parentTable;
    String dictTable,tensorTable;

    public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
        this.manager = manager;

        String baseURL = params.get("baseURL");
        Database database = manager.getDatabase(baseURL);
        fm = database.getFeaturesManager();
        degre = params.getAsInt("degre");

        mainTrainBase = params.get("trainBase");
        mainTrainTable = params.get("trainTable");

        Parameters dictParams = params.getParams("dict");
        String[] maxWordsList = dictParams.getParams("maxWords").getNames();

        Table table;
        if(params.get("input").equals("feature"))
        {
            String[] features =   params.getParams("input")
                                        .getParams("features")
                                        .getNames();

            parentBase = baseURL;
            
            for (int f = 0; f < features.length; f++) {
                parentName = features[f];
                Parameters featureDef = fm.getFeature(parentName);
                Parameters parentBagDef = featureDef.getParams("bags");
                parentBagBase = parentBagDef.get("base");
                parentBagTable = parentBagDef.get("table");
                featureDim = parentBagDef.getAsInt("dim");

                String sizeTableName = parentBagTable + "-vlat/sizes";
                dictTable = parentBagTable + "-vlat/dicts";
                tensorTable = parentBagTable + "-vlat/tensors";
                
                manager.addTable(parentBagBase,tensorTable,"ArrayList","bin");
                table = manager.addTable(parentBagBase, dictTable, "FeatureList", "bin");

                GeneratorDef generator = new GeneratorDef();
                generator.setProcess(CompVlatDict.class)
                .setParam("degre",degre)
                .setDocumentInput("input", parentBagBase, sizeTableName, null)
                .setTableInput("bags", parentBagBase, parentBagTable)
                .setTableInput("train", mainTrainBase, mainTrainTable)
                .setDocumentOutput("tensors", parentBagBase, tensorTable, null);
                table.setDocumentGenerator(generator);
                
                String pcaParamName = "";
                String valuesTable="";
                String vectorsTable="";
                String dimsTable="";
                String histTable = parentBagTable + "-vlat/";
                String format = "";
                String newFeatureName = "";
                
                if(params.get("standard").equals("yes"))
                    format = params.getParams("standard").get("format");
                
                if (dictParams.get("pca").equals("yes")){
                        pcaParamName += CompDictPca.getDescription(dictParams.getParams("pca"));
                        
                        valuesTable = parentBagTable + "-vlat/dicoEigens/values";
                        vectorsTable = parentBagTable + "-vlat/dicoEigens/vectors";
                        dimsTable = parentBagTable + "-vlat/dicoEigens/" + pcaParamName + "dims";
                        manager.addTable(parentBase, vectorsTable, "ArrayList", "bin");
                        manager.addTable(parentBase, valuesTable, "ArrayList", "bin");
                        table = manager.addTable(parentBase, dimsTable, "ArrayList");
                        
                        histTable += pcaParamName + "-";
                        
                        newFeatureName = parentName + "-"+ pcaParamName + "-vlat";
                        
                        generator = new GeneratorDef();
                        generator.setProcess(CompDictPca.class)
                            .setDocumentInput("input", parentBase, sizeTableName, null)
                            .setDocumentInput("tensor", parentBase, tensorTable, null)
                            .setParams(dictParams.getParams("pca"))
                            .setDocumentOutput("vectors", parentBase, vectorsTable, null)
                            .setDocumentOutput("values", parentBase, valuesTable, null);
                        table.setDocumentGenerator(generator);
                }
                
                Table sizeTable = manager.addTable(parentBagBase, sizeTableName, "Parameters");
                for (int mw = 0; mw < maxWordsList.length; mw++) {
                    maxWords = Integer.parseInt(maxWordsList[mw]);

                    sizeTable.setDocument(String.valueOf(maxWords),dictParams.set("maxWords",maxWords));

                    if (dictParams.get("pca").equals("yes")){
                        if (params.get("standard").equals("yes")){
                            String finalHistTable = histTable;
                            String finalNewFeatureName = newFeatureName;
                            finalHistTable += maxWords + "-" + degre;
                            finalNewFeatureName += maxWords + "-" + degre;
                            if (!format.equals("full")) {
                                finalNewFeatureName += format;
                                finalHistTable += format;
                            }
                            table = manager.addTable(parentBagBase, finalHistTable , "Feature", "bin");
                            table.setDocumentGenerator(new GeneratorDef()
                                    .setContextProcess(CompPcaVlatContext.class)
                                    .setParam("degre",degre)
                                    .setParam("format",format)
                                    .setParam("finalNewFeatureName", finalNewFeatureName)
                                    .setParam("finalHistBase", parentBase)
                                    .setParam("finalHistTable", finalHistTable)
                                    .setParams(dictParams.getParams("pca"))
                                    .setDocumentInput("input",parentBagBase,parentBagTable,null)
                                    .setDocumentInput("dict",parentBase,dictTable,String.valueOf(maxWords))
                                    .setDocumentInput("vectors", parentBase, vectorsTable,String.valueOf(maxWords))
                                    .setDocumentInput("values", parentBase, valuesTable,String.valueOf(maxWords))
                                    .setDocumentInput("dims", parentBase, dimsTable, String.valueOf(maxWords))
                                    .setDocumentInput("tensors",parentBase,tensorTable,String.valueOf(maxWords))
                            );
                        }
                    }
                    else {
                        if (params.get("standard").equals("yes") && dictParams.get("pca").equals("no"))
                            standardTables(parentName + "-vlat" + maxWords,parentBagTable + "-vlat/" + maxWords,degre,params.getParams("standard"));
                    }
                    
                    
                    
                    
                    
                    if (params.get("packed").equals("yes"))
                        packedTables(parentName + "-vlat" + maxWords,parentBagTable + "-vlat/" + maxWords,degre,params.getParams("packed"));
                    if (params.get("pca").equals("yes"))
                        pcaTables(parentName + "-vlat" + maxWords,parentBagTable + "-vlat/" + maxWords,degre,params.getParams("pca"));

                    if (params.get("sparse").equals("yes")){
                        int s = 0;
                        if(params.getParams("sparse").get("sparse").equals("dim")){
                            s = SparseVlatBuilder.sparsityStrategyDim;
                        }
                        else if(params.getParams("sparse").get("sparse").equals("energy")){
                            s = SparseVlatBuilder.sparsityStrategyEnergy;
                        }


                        int o = 0;
                        if(params.getParams("sparse").get("on").equals("diag")){
                            o = SparseVlatBuilder.sparsityOnDiag;
                        }
                        else if(params.getParams("sparse").get("on").equals("full")){
                            o = SparseVlatBuilder.sparsityOnFull;
                        }
                        else if(params.getParams("sparse").get("on").equals("value")){
                            o = SparseVlatBuilder.sparsityMaxValues;
                        }
                        sparseTables(parentName + "-vlat" + maxWords,
                        parentBagTable + "-vlat/" + maxWords,
                        degre,
                        params.getParams("sparse"),
                        o,
                        s,
                        params.getParams("sparse").getAsInt("dim"),
                        (float)params.getParams("sparse").getAsDouble("energy"));
                    }
                }
            }
        }
        else if(params.get("input").equals("image"))
        {
            String[] imagesTables =   params.getParams("input")
                                        .getParams("images")
                                        .getNames();
            
            String featureTypeName =  params.getParams("input").get("featuretype");
            Parameters featuresParams = params.getParams("input").getParams("featuretype");
            NativeDocumentGeneratorRegister.getInstance().creatpreprocessingTable(manager, baseURL, featureTypeName, featuresParams);
            
            for (int f = 0; f < imagesTables.length; f++) {
                String[] substring = imagesTables[f].split("/");
                parentDocumentName = substring[substring.length-1];
                
                parentBase = baseURL;
                parentTable = imagesTables[f];
                
                
                
                String featureName = parentDocumentName + "-" + featureTypeName;
                
                featureDim = NativeDocumentGeneratorRegister.getInstance().getDimOf(featureTypeName, featuresParams);
                
                featureName+= "_" + NativeDocumentGeneratorRegister.getInstance().getDescriptionOf(featureTypeName, featuresParams);
                
                String sizeTableName = "features/vlat/" + featureName + "/sizes";
                dictTable = "features/vlat/" + featureName + "/dicts";
                tensorTable = "features/vlat/" + featureName + "/tensors";
                
                manager.addTable(parentBase, tensorTable, "ArrayList", "bin");
                table = manager.addTable(parentBase, dictTable, "FeatureList", "bin");

                GeneratorDef generator = new GeneratorDef();
                generator.setProcess(CompVlatImageDict.class)
                    .setParam("degre", degre)
                    .setDocumentInput("input", parentBase, sizeTableName, null)
                    .setParam("featuretype", params.getParams("input").get("featuretype"), params.getParams("input").getParams("featuretype"))
                    .setTableInput("documents", parentBase, parentTable)
                    .setTableInput("train", mainTrainBase, mainTrainTable)
                    .setDocumentOutput("tensors", parentBase, tensorTable, null);
                table.setDocumentGenerator(generator);
                
                String pcaParamName = "";
                String valuesTable="";
                String vectorsTable="";
                String dimsTable="";
                String histTable="";
                String format = "";
                String newFeatureName = "";
                
                if(params.get("standard").equals("yes"))
                    format = params.getParams("standard").get("format");
                
                if(params.get("sparse").equals("yes"))
                    format = params.getParams("sparse").get("format");
                
                if (dictParams.get("pca").equals("yes")){
                        pcaParamName += CompDictPca.getDescription(dictParams.getParams("pca"));
                        
                        valuesTable = "features/vlat/" + featureName + "/dicoEigens/values";
                        vectorsTable = "features/vlat/" + featureName + "/dicoEigens/vectors";
                        dimsTable = "features/vlat/" + featureName + "/dicoEigens/" + pcaParamName + "-dims";

                        manager.addTable(parentBase, vectorsTable, "ArrayList", "bin");
                        manager.addTable(parentBase, valuesTable, "ArrayList", "bin");
                        table = manager.addTable(parentBase, dimsTable, "ArrayList");
                        
                        histTable = "features/vlat/" + featureName + "/" + pcaParamName + "-";
                        
                        newFeatureName = featureName+ "-"+ pcaParamName + "-vlat";
                        
                        generator = new GeneratorDef();
                        generator.setProcess(CompDictPca.class)
                            .setDocumentInput("input", parentBase, sizeTableName, null)
                            .setDocumentInput("tensor", parentBase, tensorTable, null)
                            .setParams(dictParams.getParams("pca"))
                            .setDocumentOutput("vectors", parentBase, vectorsTable, null)
                            .setDocumentOutput("values", parentBase, valuesTable, null);
                        table.setDocumentGenerator(generator);
                }

                Table sizeTable = manager.addTable(parentBase, sizeTableName, "Parameters");
                for (int mw = 0; mw < maxWordsList.length; mw++) {
                    
                    maxWords = Integer.parseInt(maxWordsList[mw]);
                    
                    sizeTable.setDocument(String.valueOf(maxWords),dictParams.set("maxWords", maxWords));
                    
                    if(dictParams.get("pca").equals("yes")){
                        if (params.get("standard").equals("yes")){
                            String finalHistTable = histTable;
                            String finalNewFeatureName = newFeatureName;
                            finalHistTable += maxWords + "-" + degre;
                            finalNewFeatureName += maxWords + "-" + degre;
                            if (!format.equals("full")) {
                                finalNewFeatureName += format;
                                finalHistTable += format;
                            }
                            table = manager.addTable(parentBase, finalHistTable , "Feature", "bin");
                            table.setDocumentGenerator(new GeneratorDef()
                                    .setContextProcess(CompImagePcaVlatContext.class)
                                    .setParam("degre", degre)
                                    .setParam("format", format)
                                    .setParam("finalNewFeatureName", finalNewFeatureName)
                                    .setParam("finalHistBase", parentBase)
                                    .setParam("finalHistTable", finalHistTable)
                                    .setParam("featuretype", params.getParams("input").get("featuretype"), params.getParams("input").getParams("featuretype"))
                                    .setParams(dictParams.getParams("pca"))
                                    .setDocumentInput("input", parentBase, parentTable, null)
                                    .setDocumentInput("dict", parentBase, dictTable, String.valueOf(maxWords))
                                    .setDocumentInput("vectors", parentBase, vectorsTable, String.valueOf(maxWords))
                                    .setDocumentInput("values", parentBase, valuesTable, String.valueOf(maxWords))
                                    .setDocumentInput("dims", parentBase, dimsTable, String.valueOf(maxWords))
                                    .setDocumentInput("tensors", parentBase, tensorTable, String.valueOf(maxWords))
                            );
                        }
                        
                        if (params.get("projected").equals("yes") && degre == 2){
                            String centered = "no";
                            String finalHistTable = histTable;
                            String finalNewFeatureName = newFeatureName;
                            finalHistTable += "-" + maxWords + "-projected";
                            finalNewFeatureName += maxWords + "-projected";
                            
                            if(params.getParams("projected").has("centered") && params.getParams("projected").get("centered").equals("yes")){
                                finalHistTable += "-c";
                                finalNewFeatureName += "-c";
                                centered = "yes";
                            }
                            
                            table = manager.addTable(parentBase, finalHistTable , "Feature", "bin");
                            table.setDocumentGenerator(new GeneratorDef()
                                    .setContextProcess(CompImageProjectedPcaVlatContext.class)
                                    .setParam("finalNewFeatureName", finalNewFeatureName)
                                    .setParam("finalHistBase", parentBase)
                                    .setParam("finalHistTable", finalHistTable)
                                    .setParam("centered", centered)
                                    .setParam("featuretype", params.getParams("input").get("featuretype"), params.getParams("input").getParams("featuretype"))
                                    .setParams(dictParams.getParams("pca"))
                                    .setDocumentInput("input", parentBase, parentTable, null)
                                    .setDocumentInput("dict", parentBase, dictTable, String.valueOf(maxWords))
                                    .setDocumentInput("vectors", parentBase, vectorsTable, String.valueOf(maxWords))
                                    .setDocumentInput("values", parentBase, valuesTable, String.valueOf(maxWords))
                                    .setDocumentInput("dims", parentBase, dimsTable, String.valueOf(maxWords))
                                    .setDocumentInput("tensors", parentBase, tensorTable, String.valueOf(maxWords))
                            );
                        }
                    }
                    else {
                        if (params.get("standard").equals("yes"))
                            standardTablesImage(featureName + "-vlat" + maxWords,
                            "features/vlat/" + featureName + "/" + maxWords,
                            degre,
                            params.getParams("standard"),
                            params.getParams("input").get("featuretype"),
                            params.getParams("input").getParams("featuretype"));
                        
                        if (params.get("projected").equals("yes") && degre == 2)
                            projectedTablesImage(featureName + "-vlat" + maxWords,
                            "features/vlat/" + featureName + "/" + maxWords,
                            params.getParams("input").get("featuretype"),
                            params.getParams("input").getParams("featuretype"));
                    }
                    if (params.get("sparse").equals("yes")){
                        int s = 0;
                        if(params.getParams("sparse").get("sparse").equals("dim")){
                            s = SparseVlatBuilder.sparsityStrategyDim;
                        }
                        else if(params.getParams("sparse").get("sparse").equals("energy")){
                            s = SparseVlatBuilder.sparsityStrategyEnergy;
                        }


                        int o = 0;
                        if(params.getParams("sparse").get("on").equals("diag")){
                            o = SparseVlatBuilder.sparsityOnDiag;
                        }
                        else if(params.getParams("sparse").get("on").equals("full")){
                            o = SparseVlatBuilder.sparsityOnFull;
                        }
                        else if(params.getParams("sparse").get("on").equals("value")){
                            o = SparseVlatBuilder.sparsityMaxValues;
                        }

                        sparseTablesImages(featureName + "-vlat" + maxWords,
                        "features/vlat/" + featureName + "/" + maxWords,
                        degre,
                        params.getParams("sparse"),
                        params.getParams("input").get("featuretype"),
                        params.getParams("input").getParams("featuretype"),
                        o,
                        s,
                        params.getParams("sparse").getAsInt("dim"),
                        (float)params.getParams("sparse").getAsDouble("energy"));
                    }
                }
            
            }


        }

        manager.save();
    }
    
    ////////////////////////////////////////////////////////////////////
   
    public static class CompVlatDict implements ManagerProcess {

        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Parameters qtParams = (Parameters) manager.getDocumentAs(Parameters.class, params.getParams("input"));
            Table bagTable = manager.getTable(params.getParams("bags"));
            Table trainTable = manager.getTable(params.getParams("train"));
            String[] trainKeys = trainTable.getRandomKeys(qtParams.getAsInt("maxDocs"));

            ElbgQuantizer qt = new ElbgQuantizer(qtParams);
            qt.runTable(bagTable, trainKeys);
            manager.setDocument(params.getParams("output"),  qt.getClusterCenters());
            
            ArrayList<FeatureList> tensors = new ArrayList<FeatureList>();
            int degre = params.getAsInt("degre");
            if(degre < 2)
                degre = 2;
            for (int t = 1; t <= degre; t++) {
                System.out.println("Tensor order " + t + "...");
                FeatureList tensor = qt.getClusterTensors(t);
                tensors.add(tensor);
            }
            System.out.println(tensors.size()+" tensors computed.");
            manager.setDocument(params.getParams("tensors"),  tensors);
        }
    }
    
    public static class CompDictPca implements ManagerProcess {

        public boolean isAtomic() {
            return false;
        }
        
        static public String getDescription(Parameters params) {
            String description = "pca-";
                        
            String strategy =  params.get("strategy");
            String maxDims =   params.get("maxDims");
            String maxEnergy = params.get("maxEnergy");
            String whitening = params.get("whitening");
            if(whitening.equals("yes"))
                description += "w";
            if(strategy.equals("maxEnergy")){
                description += "e" + maxEnergy;
            }else if(strategy.equals("maxDims")){
                description += "d" + maxDims;
            }else if(strategy.equals("most")){
                description += "m" + maxDims + "_" + maxEnergy;
            }else if(strategy.equals("least")){
                description += "l" + maxDims + "_" + maxEnergy;
            }
            
            return description;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            ArrayList<FeatureList> tensors = (ArrayList<FeatureList>) manager.getDocument(params.getParams("tensor"));

            FeatureList tensor2 = tensors.get(1);
            
            String strategy =   params.get("strategy");
            int maxDims =       params.getAsInt("maxDims");
            double maxEnergy =  params.getAsDouble("maxEnergy");
            
            maxEnergy /= 100.0;
            
            ArrayList<Matrix> vectorsList = new ArrayList<Matrix>();
            ArrayList<Matrix> valuesList = new ArrayList<Matrix>();
            ArrayList<Integer> dimsList = new ArrayList<Integer>();
            
            for(int i = 0 ; i < tensor2.getSize() ; i++){
                Feature covFeature = tensor2.getFeature(i);
                int dim = (int) Math.sqrt(covFeature.getDim());
                FloatMatrix cov = new FloatMatrix(covFeature.getData(), dim, dim);
                
                Pca pca = new Pca();
                pca.setMatrixCov(cov);
                pca.runPca();
                
                int dims = pca.getDimsForMaxEnergy(maxEnergy);
                
                if(strategy.equals("maxEnergy")){
                    dimsList.add(new Integer(dims));
                    
                }else if(strategy.equals("maxDims")){
                    dimsList.add(new Integer(maxDims));
                    
                }else if(strategy.equals("most")){
                    if(maxDims > dims){
                        dimsList.add(new Integer(maxDims));
                    }
                    else{
                        dimsList.add(new Integer(dims));
                    }
                }else if(strategy.equals("least")){
                    if(maxDims < dims){
                        dimsList.add(new Integer(maxDims));
                    }
                    else{
                        dimsList.add(new Integer(dims));
                    }
                    
                }
                
                vectorsList.add(pca.getDoubleVectors());
                valuesList.add(pca.getDoubleValues());
            }
            
            manager.setDocument(params.getParams("values"), valuesList);
            manager.setDocument(params.getParams("vectors"), vectorsList);
            manager.setDocument(params.getParams("output"), dimsList);
        }
        
    }

    public static class CompVlatImageDict implements ManagerProcess {

        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Parameters qtParams = (Parameters) manager.getDocumentAs(Parameters.class, params.getParams("input"));
            Table trainTable = manager.getTable(params.getParams("train"));
            String[] trainKeys = trainTable.getRandomKeys(qtParams.getAsInt("maxDocs"));

            ElbgQuantizer qt = new ElbgQuantizer(qtParams);
            
            NativeDocumentGenerator natGen =  NativeDocumentGeneratorRegister.
                    getInitialisedGenerator(manager, 
                                            params.get("featuretype"), 
                                            params.getParams("documents").get("base"),
                                            params.getParams("documents").get("table"),
                                            params.getParams("featuretype"));
            
            qt.runNativeGeneratorTable(natGen, trainTable, trainKeys);
            
            manager.setDocument(params.getParams("output"),  qt.getClusterCenters());
            
            ArrayList<FeatureList> tensors = new ArrayList<FeatureList>();
            int degre = params.getAsInt("degre");
            if(degre < 2)
                degre = 2;
            for (int t = 1; t <= degre; t++) {
                System.out.println("Tensor order " + t + "...");
                FeatureList tensor = qt.getClusterTensors(t);
                tensors.add(tensor);
            }
            System.out.println(tensors.size()+" tensors computed.");
            manager.setDocument(params.getParams("tensors"),  tensors);
        }
    }

    public static class CompVlatContext implements ManagerProcessContext {

        StandardVlatBuilder builder;
        
        NativeDocumentGenerator ngd;
        
        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            FeatureList centers = (FeatureList) manager.getDocument(params.getParams("dict"));
            ArrayList<FeatureList> tensors = (ArrayList<FeatureList>) manager.getDocument(params.getParams("tensors"));
            
            
            if(params.has("featuretype")){
                ngd =  NativeDocumentGeneratorRegister.
                    getInitialisedGenerator(manager, 
                                            params.get("featuretype"), 
                                            params.getParams("input").get("base"),
                                            params.getParams("input").get("table"),
                                            params.getParams("featuretype"));
            }
            else {
                ngd = null;
            }
                
            String pooling = params.has("pooling")?params.get("pooling"):null;
            if (pooling == null || pooling.equals("sum")) {
                builder = new StandardVlatBuilder(centers,tensors);
            }
            else if (pooling.equals("ewp")) {
                builder = new EwPooledVlatBuilder(centers,tensors,1);
            }
            else if (pooling.equals("ewn")) {
                builder = new EwPooledVlatBuilder(centers,tensors,-1);
            }
            else if (pooling.equals("cw")) {
                builder = new CwPooledVlatBuilder(centers,tensors);
            }
            else {
                throw new Exception("Invalid pooling "+pooling);
            }
        }
        
        public ManagerProcess createProcess() {
                return new CompVlat(builder, ngd);
        }

        public void dispose() {
        }
    }

    public static class CompVlat implements ManagerProcess {

        StandardVlatBuilder builder;
        
        NativeDocumentGenerator ngd;

        public CompVlat(StandardVlatBuilder builder) {
            this.builder = builder;
            this.ngd = null;
        }
        
        public CompVlat(StandardVlatBuilder builder, NativeDocumentGenerator ngd) {
            this.builder = builder;
            this.ngd = ngd;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Feature output = null;
            
            int degre = params.getAsInt("degre");
            String format = params.get("format");
            
            if(ngd==null) {
                FeatureList input = (FeatureList) manager.getDocumentAs(FeatureList.class, params.getParams("input"));
                output = builder.run(input,degre,format);
            }
            else {

                output = builder.runNativeGenerator(ngd, params.getParams("input").get("doc"), degre, format);
            }

            manager.setDocument(params.getParams("output"), output);
        }

    }
    
    
    
    ///////////////////////////////////
    
    
    public static class CompSparseVlatContext implements ManagerProcessContext {

        SparseVlatBuilder builder;
        
        NativeDocumentGenerator ngd;
        
        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            FeatureList centers = (FeatureList) manager.getDocument(params.getParams("dict"));
            ArrayList<FeatureList> tensors = (ArrayList<FeatureList>) manager.getDocument(params.getParams("tensors"));
            
            
            if(params.has("featuretype")){
                ngd =  NativeDocumentGeneratorRegister.
                    getInitialisedGenerator(manager, 
                                            params.get("featuretype"), 
                                            params.getParams("input").get("base"),
                                            params.getParams("input").get("table"),
                                            params.getParams("featuretype")); 
            }
            else {
                ngd = null;
            }

            builder = new SparseVlatBuilder(centers,tensors);
            builder.setSparsityOn(params.getAsInt("on"));
            builder.setSparsityStrategy(params.getAsInt("strategy"));
            builder.setDimSparsity(params.getAsInt("sparsedim"));
            builder.setEnergySparsity((float)params.getAsDouble("sparseenergy"));
        }
        
        public ManagerProcess createProcess() {
                return new CompSparseVlat(builder, ngd);
        }

        public void dispose() {
        }
    }

    public static class CompSparseVlat implements ManagerProcess {

        SparseVlatBuilder builder;
        
        NativeDocumentGenerator ngd;

        public CompSparseVlat(SparseVlatBuilder builder) {
            this.builder = builder;
            this.ngd = null;
        }
        
        public CompSparseVlat(SparseVlatBuilder builder, NativeDocumentGenerator ngd) {
            this.builder = builder;
            this.ngd = ngd;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Feature output = null;
            
            int degre = params.getAsInt("degre");
            String format = params.get("format");
            
            if(ngd==null) {
                FeatureList input = (FeatureList) manager.getDocumentAs(FeatureList.class, params.getParams("input"));
                output = builder.run(input,degre,format);
            }
            else {

                output = builder.runNativeGenerator(ngd, params.getParams("input").get("doc"), degre, format);
            }

            manager.setDocument(params.getParams("output"), output);
        }

    }
    
    
    
    //////////////////////////////////
    
    public static class CompProjectedVlatContext implements ManagerProcessContext {

        ProjectedVlatBuilder builder;
        
        NativeDocumentGenerator ngd;
        
        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            FeatureList centers = (FeatureList) manager.getDocument(params.getParams("dict"));
            ArrayList<FeatureList> tensors = (ArrayList<FeatureList>) manager.getDocument(params.getParams("tensors"));
            
            if(params.has("featuretype")){
                ngd =  NativeDocumentGeneratorRegister.
                    getInitialisedGenerator(manager, 
                                            params.get("featuretype"), 
                                            params.getParams("input").get("base"),
                                            params.getParams("input").get("table"),
                                            params.getParams("featuretype"));
            }
            else {
                ngd = null;
            }
                
            builder = new ProjectedVlatBuilder(centers,tensors);
        }
        
        public ManagerProcess createProcess() {
                return new CompProjectedVlat(builder, ngd);
        }

        public void dispose() {
        }
    }

    public static class CompProjectedVlat implements ManagerProcess {

        ProjectedVlatBuilder builder;
        
        NativeDocumentGenerator ngd;

        public CompProjectedVlat(ProjectedVlatBuilder builder) {
            this.builder = builder;
            this.ngd = null;
        }
        
        public CompProjectedVlat(ProjectedVlatBuilder builder, NativeDocumentGenerator ngd) {
            this.builder = builder;
            this.ngd = ngd;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Feature output = null;
            
            if(ngd==null) {
                FeatureList input = (FeatureList) manager.getDocumentAs(FeatureList.class, params.getParams("input"));
                output = builder.run(input);
            }
            else {
                output = builder.runNativeGenerator(ngd, params.getParams("input").get("doc"));
            }

            manager.setDocument(params.getParams("output"), output);
        }

    }    
    
    ////////////////////////////////////////////////////////////////////
    
    
    public static class CompImageProjectedPcaVlatContext implements ManagerProcessContext {

        ProjectedPcaVlatBuilder builder;
        
        NativeDocumentGenerator ngd;
        
        String centered;
        
        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            centered = "no";
            FeatureList centers = (FeatureList) manager.getDocument(params.getParams("dict"));
            ArrayList<FeatureList> tensors = (ArrayList<FeatureList>) manager.getDocument(params.getParams("tensors"));
            ArrayList<Matrix> vectors = (ArrayList<Matrix>) manager.getDocument(params.getParams("vectors"));
            ArrayList<Matrix> values = (ArrayList<Matrix>) manager.getDocument(params.getParams("values"));
            ArrayList<Integer> dims = (ArrayList<Integer>) manager.getDocument(params.getParams("dims"));
            
            if(params.has("centered"))
                centered = params.get("centered");
            
            FeaturesManager fm = manager.getDatabase(params.get("finalHistBase")).getFeaturesManager();
            
            
            int vlatDims = 0;
            for(int i = 0;i< dims.size();i++){
                vlatDims += dims.get(i).intValue()*dims.get(i).intValue();
            }

            fm.registerFeature(params.get("finalNewFeatureName"), 
                    new GlobalFeatureDef().setHistTable(
                        params.get("finalHistBase"),
                        params.get("finalHistTable"),
                        vlatDims)
                    );
            
            if(params.has("featuretype")) {
                ngd =  NativeDocumentGeneratorRegister.
                    getInitialisedGenerator(manager, 
                                            params.get("featuretype"), 
                                            params.getParams("input").get("base"),
                                            params.getParams("input").get("table"),
                                            params.getParams("featuretype"));
            }
            else
                ngd = null;
            
            builder = new ProjectedPcaVlatBuilder(centers, tensors, vectors, values, dims, centered);
        }
        
        public ManagerProcess createProcess() {
                return new CompImageProjectedPcaVlat(builder, ngd);
        }

        public void dispose() {
            
        }
    }

    public static class CompImageProjectedPcaVlat implements ManagerProcess {

        ProjectedPcaVlatBuilder builder;
        NativeDocumentGenerator ngd;


        public CompImageProjectedPcaVlat(ProjectedPcaVlatBuilder builder, NativeDocumentGenerator ngd) {
            this.builder = builder;
            this.ngd = ngd;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Feature output = null;
                        
            output = builder.runNativeGenerator(ngd, params.getParams("input").get("doc"));
            
            manager.setDocument(params.getParams("output"), output);
        }

    }
    
    ////////////////////////////////////////////////////////////////////
    
    public static class CompImagePcaVlatContext implements ManagerProcessContext {

        StandardPcaVlatBuilder builder;
        
        NativeDocumentGenerator ngd;
        
        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            FeatureList centers = (FeatureList) manager.getDocument(params.getParams("dict"));
            ArrayList<FeatureList> tensors = (ArrayList<FeatureList>) manager.getDocument(params.getParams("tensors"));
            ArrayList<Matrix> vectors = (ArrayList<Matrix>) manager.getDocument(params.getParams("vectors"));
            ArrayList<Matrix> values = (ArrayList<Matrix>) manager.getDocument(params.getParams("values"));
            ArrayList<Integer> dims = (ArrayList<Integer>) manager.getDocument(params.getParams("dims"));
            
            FeaturesManager fm = manager.getDatabase(params.get("finalHistBase")).getFeaturesManager();

            boolean whitening =   params.get("whitening").equals("yes");
            
            String format = params.get("format");
            int degre = params.getAsInt("degre");
            
            int vlatDims = 0;
            if(degre == 1){
                for(int i = 0;i< dims.size();i++)
                        vlatDims += dims.get(i).intValue();
            }
            else if(degre == 2){
                for(int i = 0;i< dims.size();i++){
                    if(format.equals("full")){
                        vlatDims += dims.get(i).intValue()*dims.get(i).intValue();
                    } else if(format.equals("LD")){
                        vlatDims += (dims.get(i).intValue()*(dims.get(i).intValue()+1)/2);
                    } else if(format.equals("L")){
                        vlatDims += (dims.get(i).intValue()*(dims.get(i).intValue()-1)/2);
                    }
                }
            }

            fm.registerFeature(params.get("finalNewFeatureName"), 
                    new GlobalFeatureDef().setHistTable(
                        params.get("finalHistBase"),
                        params.get("finalHistTable"),
                        vlatDims)
                    );
            
            if(params.has("featuretype")) {
                ngd =  NativeDocumentGeneratorRegister.
                    getInitialisedGenerator(manager, 
                                            params.get("featuretype"), 
                                            params.getParams("input").get("base"),
                                            params.getParams("input").get("table"),
                                            params.getParams("featuretype"));
            }
            else
                ngd = null;
            
            builder = new StandardPcaVlatBuilder(centers, tensors, vectors, values, dims, whitening);
        }
        
        public ManagerProcess createProcess() {
                return new CompImagePcaVlat(builder, ngd);
        }

        public void dispose() {
        }
    }

    public static class CompImagePcaVlat implements ManagerProcess {

        StandardPcaVlatBuilder builder;
        NativeDocumentGenerator ngd;


        public CompImagePcaVlat(StandardPcaVlatBuilder builder, NativeDocumentGenerator ngd) {
            this.builder = builder;
            this.ngd = ngd;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Feature output = null;
            
            int degre = params.getAsInt("degre");
            String format = params.get("format");
                        
            output = builder.runNativeGenerator(ngd, params.getParams("input").get("doc"), degre, format);
            
            manager.setDocument(params.getParams("output"), output);
        }

    }
    
    
    
    ///////////////////////////////////////////////////////////////////
    
    public static class CompPcaVlatContext implements ManagerProcessContext {

        StandardPcaVlatBuilder builder;
        
        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            FeatureList centers = (FeatureList) manager.getDocument(params.getParams("dict"));
            ArrayList<FeatureList> tensors = (ArrayList<FeatureList>) manager.getDocument(params.getParams("tensors"));
            ArrayList<Matrix> vectors = (ArrayList<Matrix>) manager.getDocument(params.getParams("vectors"));
            ArrayList<Matrix> values = (ArrayList<Matrix>) manager.getDocument(params.getParams("values"));
            ArrayList<Integer> dims = (ArrayList<Integer>) manager.getDocument(params.getParams("dims"));
            
            FeaturesManager fm = manager.getDatabase(params.get("finalHistBase")).getFeaturesManager();

            boolean whitening =   params.get("whitening").equals("yes");
            
            String format = params.get("format");
            int degre = params.getAsInt("degre");
            
            int vlatDims = 0;
            if(degre == 1){
                for(int i = 0;i< dims.size();i++)
                        vlatDims += dims.get(i).intValue();
            }
            else if(degre == 2){
                for(int i = 0;i< dims.size();i++){
                    if(format.equals("full")){
                        vlatDims += dims.get(i).intValue()*dims.get(i).intValue();
                    } else if(format.equals("LD")){
                        vlatDims += (dims.get(i).intValue()*(dims.get(i).intValue()+1)/2);
                    } else if(format.equals("L")){
                        vlatDims += (dims.get(i).intValue()*(dims.get(i).intValue()-1)/2);
                    }
                }
            }

            fm.registerFeature(params.get("finalNewFeatureName"), 
                    new GlobalFeatureDef().setHistTable(
                        params.get("finalHistBase"),
                        params.get("finalHistTable"),
                        vlatDims)
                    );
                
            builder = new StandardPcaVlatBuilder(centers, tensors, vectors, values, dims, whitening);
        }
        
        public ManagerProcess createProcess() {
                return new CompPcaVlat(builder);
        }

        public void dispose() {
        }
    }

    public static class CompPcaVlat implements ManagerProcess {

        StandardPcaVlatBuilder builder;

        public CompPcaVlat(StandardPcaVlatBuilder builder) {
            this.builder = builder;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Feature output = null;
            
            int degre = params.getAsInt("degre");
            String format = params.get("format");
            
            FeatureList input = (FeatureList) manager.getDocumentAs(FeatureList.class, params.getParams("input"));
            
            
            output = builder.run(input,degre,format);


            manager.setDocument(params.getParams("output"), output);
        }

    }
    
    ///////////////////////////////////////////////////////////////////

    public static class CompPackedVlatContext implements ManagerProcessContext {

        PackedVlatBuilder builder;

        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            builder = new PackedVlatBuilder(
                    (FeatureList)manager.getDocument(params.getParams("dict")),
                    (List) manager.getDocument(params.getParams("tensors"))
            );
        }

        public ManagerProcess createProcess() {
            return new CompPackedVlat(builder);
        }

        public void dispose() {
        }
    }

    public static class CompPackedVlat implements ManagerProcess {

        PackedVlatBuilder builder;

        public CompPackedVlat(PackedVlatBuilder builder) {
            this.builder = builder;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            FeatureList input = (FeatureList) manager.getDocumentAs(FeatureList.class, params.getParams("input"));
            int degre = params.getAsInt("degre", 2);
            double mse = params.getAsDouble("mse", 0);
            Object output = builder.run(input, degre, mse);
            manager.setDocument(params.getParams("output"), output);
        }

    }

    public static class CompVlatPcaDict implements ManagerProcess {

        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            Parameters pcaParams = (Parameters) manager.getDocumentAs(Parameters.class, params.getParams("input"));
            Table table = manager.getTable(params.getParams("bags"));
            FeatureList centers = (FeatureList)manager.getDocument(params.getParams("dict"));

            CwPcaVlatBuilder pca = new CwPcaVlatBuilder(centers);
            pca.compDict(table, table.getRandomKeys(pcaParams.getAsInt("maxSamples")), pcaParams.getAsInt("maxDim"), pcaParams.getAsDouble("maxEnergy"));

            manager.setDocument(params.getParams("output"), pca.getDirs());
            manager.setDocument(params.getParams("pcameans"), pca.getMeans());
            manager.setDocument(params.getParams("pcavars"), pca.getVars());
        }

    }

    public static class CompVlatPcaTensor implements ManagerProcess {

        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {

            int degre = params.getAsInt("degre");
            String format = params.get("format");
            Table table = manager.getTable(params.getParams("bags"));
            CwPcaVlatBuilder builder = new CwPcaVlatBuilder(
                    (FeatureList)manager.getDocument(params.getParams("dict")),
                    (List) manager.getDocument(params.getParams("pcadicts")),
                    (List) manager.getDocument(params.getParams("pcameans")),
                    (List) manager.getDocument(params.getParams("pcavars"))
            );
            Feature tensor = builder.compMeanVlat(table, table.getKeys(), degre, format);
            manager.setDocument(params.getParams("output"),tensor);
        }

    }

    public static class CompVlatPcaContext implements ManagerProcessContext {

        CwPcaVlatBuilder builder;

        public boolean isAtomic() {
            return false;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            builder = new CwPcaVlatBuilder(
                    (FeatureList)manager.getDocument(params.getParams("dict")),
                    (List) manager.getDocument(params.getParams("pcadicts")),
                    (List) manager.getDocument(params.getParams("pcameans")),
                    (List) manager.getDocument(params.getParams("pcavars"))
            );
        }

        public ManagerProcess createProcess() {
            return new CompVlatPca(builder);
        }

        public void dispose() {
        }
    }

    public static class CompVlatPca implements ManagerProcess {

        CwPcaVlatBuilder builder;

        public CompVlatPca(CwPcaVlatBuilder builder) {
            this.builder = builder;
        }

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
            FeatureList input = (FeatureList) manager.getDocumentAs(FeatureList.class, params.getParams("input"));
            int degre = params.getAsInt("degre");
            String format = params.get("format");
            Feature output = builder.compVlat(input,degre,format);

            //MultiPrincipalComponentAnalysis pca = new MultiPrincipalComponentAnalysis(
            //        (List) manager.getDocument(params.getParams("pcadicts")),
            //        (List) manager.getDocument(params.getParams("pcameans")),
            //        (List) manager.getDocument(params.getParams("pcavars")));
            //Feature check = pca.compVLAT((FeatureList)manager.getDocument(params.getParams("dict")), input, degre);
            //float diff = Feature.distInf(check,output);
            //System.out.println("diff "+diff);
            //if (diff > 1E-5) {
            //    System.out.println("Error standard vlat "+diff);
            //    Random rand = new Random();
            //    for (int i=0;i<10;i++) {
            //        int j = i; // rand.nextInt(output.getDim());
            //        System.out.println(j+":"+check.getValue(j)+" != "+output.getValue(j)+";  ");
            //    }
            //    System.out.println();
            //}

            manager.setDocument(params.getParams("output"), output);
        }

    }
/*
    public static int compTensorDim(int n, int degre) throws Exception {
        int dim = 1;
        for (int d = 0; d < degre; d++) {
            dim *= (n - d);
        }
        for (int d = 0; d < degre; d++) {
            dim /= (d + 1);
        }

        if (degre == 1) {
            if (n != dim) {
                throw new Exception("Error tensor dim 1");
            }
        } else if (degre == 2) {
            int k = 0;
            for (int r = 0; r < n; r++) {
                for (int s = r + 1; s < n; s++) {
                    k++;
                }
            }
            if (k != dim) {
                throw new Exception(String.format("Error tensor dim 2 %d != %d", k, dim));
            }
        } else if (degre == 3) {
            int k = 0;
            for (int r = 0; r < n; r++) {
                for (int s = r + 1; s < n; s++) {
                    for (int t = s + 1; t < n; t++) {
                        k++;
                    }
                }
            }
            if (k != dim) {
                throw new Exception(String.format("Error tensor dim 3 %d != %d", k, dim));
            }
        } else {
            throw new Exception(String.format("Error %d", degre));
        }
        return dim;
    }*/

}
