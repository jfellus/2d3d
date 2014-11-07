package retin.feature.vlat;

import java.util.List;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.basic.NativePointer;
import retin.toolbox.document.list.FeatureList;

public class ProjectedVlatBuilder {

    protected FeatureList centers;
    protected List tensors;

    public ProjectedVlatBuilder(FeatureList centers,List tensors) {
        this.centers = centers;
        this.tensors = tensors;
    }

    public Feature    runNativeGenerator (NativeDocumentGenerator ngd, String docKey){
        return runNativeGenerator(ngd.getInstance(), docKey);
    }
    
    public native Feature    run (FeatureList input);
     
    private native Feature   runNativeGenerator(NativePointer instance, String docKey);


    public static int               getStandardDim(int featureDim) {
          return featureDim*featureDim;
    }

    public static int               getDim(int dictSize, int featureDim) { return dictSize*getStandardDim(featureDim); }
}
