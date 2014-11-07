package retin.feature.vlat;

import java.util.List;
import retin.toolbox.document.list.FeatureList;

public class PackedVlatBuilder {

    protected FeatureList centers;
    protected List tensors;

    public PackedVlatBuilder(FeatureList centers,List tensors) {
        this.centers = centers;
        this.tensors = tensors;
    }

    public native Object    run (FeatureList input,int order,double mse);


}
