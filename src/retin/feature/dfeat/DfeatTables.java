
package retin.feature.dfeat;

import java.io.PrintWriter;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import retin.feature.color.ImageColor;
import retin.toolbox.database.Database;
import retin.toolbox.database.FeaturesManager;
import retin.toolbox.database.Manager;
import retin.toolbox.database.Table;
import retin.toolbox.database.database_operations.DatabaseOperation;
import retin.toolbox.database.featuredef.BagFeatureDef;
import retin.toolbox.database.generator.GeneratorDef;
import retin.toolbox.database.processing.ManagerProcess;
import retin.toolbox.document.BytesMatrix;
import retin.toolbox.document.ImageFile;
import retin.toolbox.document.Options;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.list.FeatureList;
import retin.toolbox.document.list.KeypointList;

/**
 *
 * @author picard
 */
public class DfeatTables extends DatabaseOperation {

    public DfeatTables() {
        super("DfeatTables", "Features/DfeatTables");
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
    public void fillOptions(Manager manager, Options options, Database database, AbstractMap<String, String> args) {
        options.addString("sourceTable").setDefault("documents/equalization");
        options.addInteger("spacing").setRange(1, 100).setDefault(20);
        options.addStringValues("scales").addDefault("4");
        //options.addStringValues("scales").addDefault("4").addDefault("6").addDefault("8").addDefault("10");
        options.addStringSelection("type").add("hog").add("fastsift").add("sift").setDefault("hog");
    }



    @Override
    public void run(PrintWriter out, Manager manager, Database database, Parameters params) throws Exception {



        String[] stringscales = params.getParams("scales").getNames();
        int step = params.getAsInt("spacing");
        String fullName = "dfeat-dsift-"+step+""+Arrays.toString(stringscales).replaceAll(" ", "").replaceAll("," , "+").replaceAll("\\[", "@").replaceAll("\\]", "");
        String tablesPath = "features/dfeat/dsift-"+step+""+ Arrays.toString(stringscales).replaceAll(" ", "").replaceAll("," , "+").replaceAll("\\[", "@").replaceAll("\\]", "");



        FeaturesManager fm = database.getFeaturesManager();
        String bagsTable = tablesPath+"/bags";
        fm.registerFeature (fullName+"-sift",new BagFeatureDef(fullName)
                        .setBagTable(database.getURL(),bagsTable,128));


        Table descTable = database.addTable (bagsTable,"FeatureList","bin");
        System.out.println("params : "+params.toString());
        descTable.setDocumentGenerator(new GeneratorDef()
                .setProcess(Extract.class)
				.setParams(params)
                .setDocumentInput("input", params.get("sourceTable"))
                .setDocumentOutput("output", bagsTable)
        );


        manager.save ();
    }


    public static class Extract implements ManagerProcess {

        public boolean isAtomic() {
            return true;
        }

        public void process(PrintWriter out, Manager manager, Parameters params) throws Exception {
try {


            List<KeypointList> klist = new ArrayList<KeypointList>();
            ArrayList<Feature> dlist = new ArrayList<Feature>();

			ImageFile input = (ImageFile) manager.getDocumentAs(ImageFile.class, params.getParams("input"));
            String imageFile = manager.getTable(params.getParams("input").get("base"),params.getParams("input").get("table")).getDataDir();
            imageFile = imageFile + "/" + input.getName();

			DaveFeature df = new DaveFeature();
			FeatureList desc = null;
			if(params.get("type").equals("hog"))
				desc = df.extractHog(imageFile, params);
			else if(params.get("type").equals("fastsift"))
				desc = df.extractFastSift(imageFile, params);
			else if(params.get("type").equals("sift"))
				desc = df.extractDSift(imageFile, params);
			else
				System.out.println("no params !!!");

			if(desc != null)
				manager.setDocument(params.getParams("output"), desc);
}
catch(Exception e) {
	e.printStackTrace();
}
        }
    }

}
