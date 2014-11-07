
package retin.toolbox.quantization;

import retin.toolbox.core.Library;
import retin.toolbox.database.Table;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.document.FeatureMatrix;
import retin.toolbox.document.IndexMatrix;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.basic.NativePointer;
import retin.toolbox.document.list.FeatureList;

public class ElbgQuantizer {
    static
    {
        Library.load("toolbox_quantization");
    }

    protected Parameters params;
    protected long lbgPtr;
    protected long gmmPtr;
    protected long spacePtr;

    public ElbgQuantizer(String params) throws Exception {
        this.lbgPtr = 0;
        this.gmmPtr = 0;
        this.spacePtr = 0;
        this.params = new Parameters(params);
    }

    public ElbgQuantizer(Parameters params) {
        this.lbgPtr = 0;
        this.gmmPtr = 0;
        this.spacePtr = 0;
        this.params = params;
    }

    protected void finalize() {
        release();
    }
    protected native void release();

    public void runNativeGeneratorTable(NativeDocumentGenerator ngd, Table table, String[] keys){
        runNativeGeneratorTable(ngd.getInstance() , table, keys);
    }

    public native void runFeatureList(FeatureList input);
    public native void runFeatureMatrix(FeatureMatrix input);
    public native void runTable(Table table, String[] keys);
    private native void runNativeGeneratorTable(NativePointer instance, Table table, String[] keys);
    public native void runSampledTable(Table table, String[] keys, int percent);
    public native double getDistortion();
    public native FeatureList getClusterCenters();
    public native Feature getClusterSizes();
    public native Feature getClusterDistortions();
    public native Feature getClusterVariances();
    public native FeatureList getClusterTensors(int order);
    public native int[] getLabels();
    public native IndexMatrix getIndexMatrix(int width, int height);
    public native FeatureMatrix getFeatureMatrix(int width, int height);

    public native Feature getGaussianWeights();
    public native FeatureList getGaussianMeans();
    public native FeatureList getGaussianVars();
}


