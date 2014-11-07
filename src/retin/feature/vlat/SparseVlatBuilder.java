package retin.feature.vlat;

import java.util.ArrayList;
import java.util.List;
import retin.toolbox.document.FloatMatrix;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.basic.NativePointer;
import retin.toolbox.document.list.FeatureList;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author romanegr
 */
public class SparseVlatBuilder {
    public static int sparsityStrategyDim = 0;
    public static int sparsityStrategyEnergy = 1;
    
    public static int sparsityOnDiag = 0;
    public static int sparsityOnFull = 1;
    public static int sparsityMaxValues = 2;
    
    protected FeatureList centers;
    protected List tensors;
    
    protected List<FloatMatrix> Ls;
    protected List<FloatMatrix> Ps;
    
    protected int sparsityOn;
    protected int sparsityStrategy;
    protected int sparsityDim;
    protected float sparsityEnergy;

    public SparseVlatBuilder(FeatureList centers,List tensors) throws Exception {
        this.centers = centers;
        this.tensors = tensors;
        FeatureList Covs = (FeatureList) tensors.get(1);
        Ls = new ArrayList<FloatMatrix>();
        Ps = new ArrayList<FloatMatrix>();
        
        for(int i = 0 ; i < Covs.getSize() ; i++){
            FloatMatrix cov = new FloatMatrix(Covs.getFeature(i).getData(), centers.getDim(), centers.getDim());
            Pca pca = new Pca();
            pca.setMatrixCov(cov);
            pca.runPca();
            Ls.add(pca.getValues());
            Ps.add(pca.getVectors());
        }
        
        sparsityStrategy = sparsityStrategyDim;
        sparsityOn = sparsityOnDiag;
        sparsityDim = 0;
        sparsityEnergy = 1.0f;
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
    
    void setSparsityOn(int o){
        if(o == sparsityOnDiag ||
                o == sparsityOnFull){
            sparsityOn = o;
        }
    }
    
    void setSparsityStrategy(int s){
        if(s == sparsityStrategyDim ||
                s == sparsityStrategyEnergy){
            sparsityStrategy = s;
        }
    }
    
    void setEnergySparsity(float e){
        if(e > 0 && e <= 1.0)
            sparsityEnergy = e;
    }
    
    void setDimSparsity(int d){
        if(d > 0)
            sparsityDim = d;
    }

    public static int               getDim(int dictSize,int order,int featureDim,String format) { return dictSize*getStandardDim(order, featureDim, format); }
    
}
