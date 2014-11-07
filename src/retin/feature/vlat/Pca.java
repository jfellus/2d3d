/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.feature.vlat;

import retin.toolbox.document.FloatMatrix;
import retin.toolbox.document.Matrix;

/**
 *
 * @author romanegr
 */
public class Pca {
    protected FloatMatrix cov, vectors, values;
    
    public Pca(){
        cov = null;
        vectors = null;
        values = null;
    }
    
    public void setMatrixCov(FloatMatrix cov){
        this.cov = cov;
        vectors = new FloatMatrix(cov.getVectorSize(), cov.getVectorCount());
        values = new FloatMatrix(cov.getVectorSize(),1);
    }
    
    public void runPca() throws Exception{
        if((cov == null) || (vectors == null) || (values == null)){
             throw new Exception("est null");
        }
        if(cov.getVectorCount() != cov.getVectorSize())
            throw new Exception("non carré");
        runNativePca();
    }
    protected native void runNativePca();
    
    public FloatMatrix getVectors(){
        return vectors;
    }
    
    public FloatMatrix getValues(){
        return values;
    }
    
    public Matrix getDoubleVectors(){
        return new Matrix(vectors);
    }
    
    public Matrix getDoubleValues(){
        return new Matrix(values);
    }
    
    public int getDimsForMaxEnergy(double maxEnergy){
        int dims = 0;
        double energy = 0.0;
        
        double energySum = 0.0;
        
        
        if((cov == null) || (vectors == null) || (values == null)){
             return dims;
        }
        
        for(int i = 0 ; i < values.getVectorSize(); i++){
            energySum += values.getValue(i,0);
        }
        
        while(energy < maxEnergy && dims < values.getVectorSize()){
            energy += values.getValue(dims, 0)/energySum;
            dims++;
        }
        
        if(maxEnergy == 1.0)
            return values.getVectorSize();
        return dims;
    }
}
