/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.feature.vlat;

import java.util.List;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.basic.NativePointer;
import retin.toolbox.document.list.FeatureList;

/**
 *
 * @author romanegr
 */
public class ProjectedPcaVlatBuilder {
    protected FeatureList centers;
    protected List tensors;
    protected List vectors;
    protected List values;
    protected List dims;
    
    protected String centered;
    
    ProjectedPcaVlatBuilder(FeatureList centers, List tensors, List vectors, List values, List dims, String centered){
        this.centers = centers;
        this.tensors = tensors;
        this.vectors = vectors;
        this.values = values;
        this.dims = dims;
        this.centered = centered;
    }
    
    public Feature    runNativeGenerator (NativeDocumentGenerator ngd, String docKey){
        return runNativeGenerator(ngd.getInstance(), docKey);
    }
      
    public native Feature    run(FeatureList input,int order,String format);
    
    private native Feature   runNativeGenerator(NativePointer instance, String docKey);
}
