/**
 * \file Feature.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.basic;

import retin.toolbox.document.Document;
import retin.toolbox.document.IndexMatrix;

public class Feature extends Document
{
	private int dim;
	private float[] data;

	public	Feature ()
	{
		this.dim = 0;
	}
	public Feature (int dim)
	{
		this.dim = dim;
		this.data = new float[dim];
	}
	public Feature (float[] data)
	{
		this.dim = data.length;
		this.data = new float[dim];
		System.arraycopy (data,0,this.data,0,dim);
	}
        public Feature (Feature src, int [] index)
	{
		this.dim = index.length;
		this.data = new float[dim];
		for(int i = 0 ; i < dim ; i++){
                    data[index[i]] = src.getValue(index[i]);
                }
	}
	public Feature (IndexMatrix map)
	{
		this.dim = map.getDim();
		this.data = new float[dim];
		int[] indexes = map.getIndexes();
		for (int i=0;i<indexes.length;i++)
			data[ indexes[i] ] ++;
	}

	public int getDim () { return dim; }
	public int getLength () { return data.length; }
	public float[] getData() { return data; }

	public float getValue(int i) { return data[i]; }
	public void setValue(int i,float x) { data[i] = x; }
	public void incValue(int i) { data[i]++; }
        
        public void trinarise() { 
            for (int i=0;i<dim;i++)
		data[i] = java.lang.Math.signum(data[i]);
        }
        
        public void binarise() { 
            for (int i=0;i<dim;i++)
            {
                if(data[i] < 0)
                    data[i] = -1.0f;
                else
                    data[i] = 1.0f;
            }
        }

	public void add(Feature feature) {
		if (feature.dim != dim)
			throw new IllegalArgumentException("Mismatch dims");
		for (int i=0;i<dim;i++)
			this.data[i] += feature.data[i];
	}
        
        public void add(float f[], int offset) {
		if (f.length < offset + dim)
			throw new IllegalArgumentException("Mismatch dims");
		for (int i=0;i<dim;i++)
			this.data[i] += f[i+offset];
	}
        
        public void addValue(int i, float x) {
            if (i >= dim)
                throw new IllegalArgumentException("Mismatch index");
            this.data[i] += x;
	}

	public float getNormL2() {
		double n = 0;
		for (int i=0;i<dim;i++)
			n += data[i]*data[i];
		return (float) Math.sqrt(n);
	}
	
	public float getNormL1() {
		double n = 0;
		for (int i=0;i<dim;i++)
			n += Math.abs(data[i]);
		return (float) n;
	}

    public float dotprod (Feature f) {
        if (f.dim != dim)
                throw new IllegalArgumentException("Mismatch dims");
        float sum = 0;
        for (int i=0;i<dim;i++)
            sum += data[i]*f.data[i];
        return sum;
    }

	public void div(float x) {
	    if(x == 0.0f)
	        throw new ArithmeticException("Divide by zero");
            x = 1/x;
            for (int i=0;i<dim;i++)
                    data[i] *= x;
	}
        
        public void mul(float x) {
            for (int i=0;i<dim;i++)
                    data[i] *= x;
	}


	public void resize (int newdim) {
		float[] newdata = new float[newdim];
		System.arraycopy(this.data,0,newdata,0,Math.min(dim,newdim));
		this.data = newdata;
		this.dim = newdim;
	}

	public int getArgMax() {
		int m = 0;
		for (int i=1;i<data.length;i++) {
			if (data[i] > data[m]) m = i;
		}
		return m;
	}

	public void concat (Feature addFeature)
	{
                if (addFeature == null || addFeature.dim == 0)
                    return;
		float[] newData = new float[dim+addFeature.dim];
                if (this.data != null)
                    System.arraycopy(this.data,0,newData,0,this.dim);                
		System.arraycopy(addFeature.data,0,newData,this.dim,addFeature.dim);
		this.dim+=addFeature.getDim();
		this.data=newData;

	}

	public boolean equals(Object o) {
		Feature x = (Feature)o;
		if (x.dim != dim)
			return false;
		for (int k=0;k<dim;k++)
			if (Math.abs(x.data[k]-data[k]) > 1E-7f)
				return false;
		return true;
	}

        public static float distInf (Feature a,Feature b) {
            if (a.dim != b.dim)
                throw new IllegalArgumentException("Different dims");
            float r = 0;
            for (int k=0;k<a.dim;k++)
                r = Math.max(r,Math.abs(a.getValue(k)-b.getValue(k)));
            return r;
        }

	public void show () 
	{
		for (int i=0;i<dim;i++)
			System.out.print(data[i]+", ");
		System.out.println(); 
	}

    public Feature getSubFeature(int i, int dim) {
        Feature f = new Feature(dim);
        System.arraycopy(this.data, i, f.data, 0, dim);
        return f;
    }

    public void pow(double power) {
        pow(power, true);
    }
    
    public void pow(double power, boolean keepSign) {
        if (power == 1)
            return;
        if(keepSign){
            for (int i=0;i<dim;i++)
                if(data[i]>=0)
                    data[i] = (float) Math.pow(data[i], power);
                else
                    data[i] = (float) -Math.pow(-data[i], power);
        }
        else {
            for (int i=0;i<dim;i++)
                data[i] = (float) Math.pow(Math.abs(data[i]), power);
        }
    }
    
    public void normalize(double power, String normalize) {
        pow(power);
        if (normalize == null || normalize.equals("none"))
            return;
        float norm;
        if (normalize.equals("l2"))
            norm = getNormL2();
        else
            throw new IllegalArgumentException("Invalid norm "+normalize);
        if (Math.abs(norm) < 1E-5)
            return;
        div(norm);
    }


}
