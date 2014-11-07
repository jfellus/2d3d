package retin.feature.vlat;

import java.util.List;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.list.FeatureList;

public class CwPooledVlatBuilder extends StandardVlatBuilder {

    public CwPooledVlatBuilder(FeatureList centers,List tensors) {
        super(centers,tensors);
    }
    
    @Override
    public native Feature    run (FeatureList input,int order,String format);
    
    public native Feature    runImage (String imageFile,int order,String format, String featuretype, Parameters featureParams);


}
