/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.feature.vlat;

import java.util.ArrayList;
import java.util.List;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.basic.NativePointer;
import retin.toolbox.document.list.FeatureList;

/**
 *
 * @author romanegr
 */
public class StandardPcaVlatBuilder {
    protected FeatureList centers;
    protected List tensors;
    protected List vectors;
    protected List values;
    protected List dims;
    protected boolean whitening;
    
    public StandardPcaVlatBuilder(FeatureList centers, List tensors, List vectors, List values, List dims, boolean whitening){
        this.centers = centers;
        this.tensors = tensors;
        this.vectors = vectors;
        this.values = values;
        this.dims = dims;
        this.whitening = whitening;

    }
    
    public Feature    runNativeGenerator (NativeDocumentGenerator ngd, String docKey, int order, String format){
        return runNativeGenerator(ngd.getInstance(), docKey, order, format);
    }
      
    public native Feature    run(FeatureList input,int order,String format);
    
    private native Feature   runNativeGenerator(NativePointer instance, String docKey, int order, String format);
}
