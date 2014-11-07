package retin.feature.vlat;

import java.util.List;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.list.FeatureList;

public class EwPooledVlatBuilder extends StandardVlatBuilder {

    protected float sign;
    
    public EwPooledVlatBuilder(FeatureList centers,List tensors,float sign) {
        super(centers,tensors);
        this.sign = sign;
    }
    
    @Override
    public native Feature    run (FeatureList input,int order,String format);
    
    public native Feature    runImage (String imageFile,int order,String format, String featuretype, Parameters featureParams);


}
