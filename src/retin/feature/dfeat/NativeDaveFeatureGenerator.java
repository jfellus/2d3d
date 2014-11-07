/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.feature.dfeat;

import java.util.AbstractMap;
import retin.toolbox.database.Manager;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.document.Options;
import retin.toolbox.document.Parameters;
import retin.toolbox.nativegenerator.NativeFeatureListGenerator;

/**
 *
 * @author romanegr
 */
public class NativeDaveFeatureGenerator extends NativeFeatureListGenerator {

    public static String NAME = "dfeat";
    
    public NativeDaveFeatureGenerator(){
        newIntance();
    }
    
    @Override
    protected void finalize() throws Throwable{
        deleteIntance();
        super.finalize();
    }
    
    @Override
    public String getName() {
        return NAME;
    }

    @Override
    public String[] getSupportedInputType() {
        String list[] = {NativeDocumentGenerator.IMAGETYPE};
        return list;
    }

    @Override
    public Options getOptions(Manager manager, AbstractMap<String, String> args) {
        Options dfeat = new Options();
        
        dfeat.addInteger("spacing").setRange(1, 100).setDefault(6);
        dfeat.addStringValues("scales").addDefault("4").addDefault("6").addDefault("8").addDefault("10");
        dfeat.addStringSelection("addscalesindescr").add("yes").add("no").setDefault("no");
        dfeat.addDouble("minX").setRange(0.0, 1.0).setDefault(0.0);
        dfeat.addDouble("minY").setRange(0.0, 1.0).setDefault(0.0);
        dfeat.addDouble("maxX").setRange(0.0, 1.0).setDefault(1.0);
        dfeat.addDouble("maxY").setRange(0.0, 1.0).setDefault(1.0);
        dfeat.addInteger("maxSize").setRange(0, 1024).setDefault(0);
        dfeat.addStringSelection("type").add("hog").add("fastsift").add("sift").add("rgb").setDefault("hog");
        
        return dfeat;
    }

    @Override
    public String getDescription(Parameters params) {
        String description = "";
        
        if(params.has("maxSize") && params.getAsInt("maxSize", 0) > 0)
            description += "r" + params.getAsInt("maxSize", 0);
        
        if(params.has("type"))
            description += params.get("type");
        if(params.has("spacing"))
            description += params.getAsInt("spacing")+"@";
        if(params.has("addscalesindescr") && params.get("addscalesindescr").equals("yes")){
            String scales[] = params.getParams("scales").getNames();
            for(int i = 0 ; i < scales.length ; i++){
                if(i > 0)
                    description += "+";
                description += scales[i];
            }
        }
            
        try {
            if(params.has("minX") && params.getAsDouble("minX")!=0.0)
            {
                description += "mx"+params.get("minX");
            }
            if(params.has("minY") && params.getAsDouble("minY")!=0.0)
            {
                description += "my"+params.get("minY");
            }
            if(params.has("maxX")  && params.getAsDouble("maxX")!=1.0)
            {
                description += "Mx"+params.get("maxX");
            }
            if(params.has("maxY") && params.getAsDouble("maxY")!=1.0)
            {
                description += "My"+params.get("maxY");
            }
        } catch (Exception ex) {
            System.out.println(ex.toString());
        }
        
        return description;
    }
    
    private native void newIntance();
    private native void deleteIntance();
}
