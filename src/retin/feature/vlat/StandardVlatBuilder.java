package retin.feature.vlat;

import java.util.List;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.basic.NativePointer;
import retin.toolbox.document.list.FeatureList;

public class StandardVlatBuilder {

    protected FeatureList centers;
    protected List tensors;

    public StandardVlatBuilder(FeatureList centers,List tensors) {
        this.centers = centers;
        this.tensors = tensors;
    }

    public native Feature    run (FeatureList input,int order,String format);
        
    public Feature    runNativeGenerator (NativeDocumentGenerator ngd, String docKey, int order, String format){
        return runNativeGenerator(ngd.getInstance(), docKey, order, format);
    }
    
    private native Feature   runNativeGenerator(NativePointer instance, String docKey, int order, String format);

    public int dim(int order,String format) {
        return getDim(centers.getSize(),order,centers.getDim(),format);
    }
    
    public static int               getStandardDim(int order,int featureDim,String format) {
        if (order == 1) {
            return featureDim;
        }
        else if (order == 2) {
            if (format.equals("full"))
                return featureDim*featureDim;
            else if (format.equals("L"))
                return (featureDim*(featureDim-1))/2;
            else if (format.equals("LD"))
                return (featureDim*(featureDim+1))/2;
            else
                throw new IllegalArgumentException("Unsupported format");
        }
        else
            throw new IllegalArgumentException("Unsupported order");
    }

    public static int               getDim(int dictSize,int order,int featureDim,String format) { return dictSize*getStandardDim(order, featureDim, format); }


}
