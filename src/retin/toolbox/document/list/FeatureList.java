/**
 * \file FeatureList.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.list;

import java.awt.image.*;
import java.util.ArrayList;
import java.util.Arrays;
import retin.toolbox.document.*;
import retin.toolbox.document.basic.Feature;

public class FeatureList extends Document
{
	private int dim;
	private int size;
	private float[] features;

	public	FeatureList ()
	{
		this.size = 0;
		this.dim = 0;
	}
	public FeatureList (int dim,int size)
	{
		this.size = size;
		this.dim = dim;
		this.features = new float[dim*size];
	}
        public FeatureList (float [] b, int dim,int size)
	{
		this.size = size;
		this.dim = dim;
		this.features = new float[dim*size];
                System.arraycopy (b,0,this.features,0,dim*size);
	}
	public FeatureList (FeatureList list,int[] indexes) {
		size = indexes.length;
		dim = list.dim;
		features = new float[dim*size];
		for (int i=0;i<size;i++)
			System.arraycopy(list.features,dim*indexes[i],features,dim*i,dim);
	}
	public FeatureList (ArrayList<Feature> list) {
		size = list.size();
		dim = list.get(0).getDim();
		features = new float[dim*size];
		for (int i=0;i<size;i++)
			System.arraycopy(list.get(i).getData(),0,features,dim*i,dim);
	}
	public FeatureList (FeatureMatrix map)
	{
		this.dim = map.getDim();
		this.size = map.getWidth()*map.getHeight();
		this.features = new float[dim*size];
		System.arraycopy (map.getFeatures(),0,features,0,dim*size);
	}
	public FeatureList (BytesList list)
	{
		dim = list.getDim();
		size = list.getSize();
		features = new float[dim*size];
		byte[] bytes = list.getBytes();
		for (int i=0;i<features.length;i++) {
			features[i] = ((int)bytes[i]) & 0xFF;
		}
	}
	public FeatureList (BufferedImage image) throws Exception
	{
		size = image.getWidth()*image.getHeight();

		if (image.getType() == BufferedImage.TYPE_3BYTE_BGR)
		{
			dim = 3;
			features = new float[size*dim];
			byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			for (int i=0;i<size;i++)
			{
				features[3*i+0] = (((int)buffer[3*i+2]) & 0xFF) / 255.0f;
				features[3*i+1] = (((int)buffer[3*i+1]) & 0xFF) / 255.0f;
				features[3*i+2] = (((int)buffer[3*i+0]) & 0xFF) / 255.0f;
			}
		}
		else
			throw new Exception ("FeatureList(BufferedImage) Unsupported format");
	}

	public void add(int i, Feature feature) {
		if (feature.getDim() != dim)
			throw new IllegalArgumentException("Invalid dim");
		if (i < 0 || i >= size)
			throw new IllegalArgumentException("Invalid index");
		for (int k=0;k<dim;k++)
			features[i*dim+k] += feature.getValue(k);
	}
	public void div(int i, float x) {
		if (x == 0)
			return;
		if (i < 0 || i >= size)
			throw new IllegalArgumentException("Invalid index");
		for (int k=0;k<dim;k++)
			features[i*dim+k] /= x;
	}

	public int getDim () { return dim; }
	public int getSize () { return size; }
	public void	setFeatures (float[] features) { this.features = features; }
	public float[] 	getFeatures() { return features; }

	public	Feature	getFeature (int i) 
	{
		Feature f = new Feature (dim);
		System.arraycopy (features,i*dim,f.getData(),0,dim);
		return f;
	}
        
        public	Feature	getAggregFeature (Feature p) 
	{
            if(p.getDim() != size)
                throw new IllegalArgumentException("Invalid dim "+size+" != "+p.getDim());
            
            Feature f = new Feature (dim);
            for(int j = 0; j < dim ; j++){
                f.setValue(j, 0.0f);
            }
            for(int i = 0 ; i < size ; i++){
                float w = p.getValue(i);
                if(w != 0.0f){
                    for(int j = 0; j < dim ; j++){
                        f.addValue(j, w * features[i*dim+j]);
                    }
                }
            }
            return f;
	}
        
        public float getNormL2(int i) {
		double n = 0;
		for (int j=0;j<dim;j++)
			n += features[i*dim+j]*features[i*dim+j];
		return (float) Math.sqrt(n);
	}
        
	public	void	setFeature (int i,Feature f)
	{
		if (i < 0 || i >= size)
			throw new IllegalArgumentException("Invalid index "+i);
		if (f.getDim() != dim)
			throw new IllegalArgumentException("Invalid dim "+dim+" != "+f.getDim());
		System.arraycopy (f.getData(),0,features,i*dim,dim);
	}

	public	void	setFeatureList (int i,FeatureList set)
	{
		System.arraycopy (set.features,0,features,i*dim,set.size*dim);
	}

	public	FeatureList getSubDims (int d0,int dn)
	{
		FeatureList list = new FeatureList(dn,size);
		for (int j=0;j<size;j++) {
			for (int d=0;d<dn;d++)
				list.features[d+j*dn] = features[d0+d+j*dim];
		}
		return list;
	}

	public void setValue (int k,int i,float x) 
	{
		features[k+i*dim] = x;
	}
	public float getValue (int k,int i) 
	{
		return features[k+i*dim];
	}
	public void incValue (int k,int i) 
	{
		features[k+i*dim] ++;
	}
	public void divValue (int k,int i,float x)
	{
		if (x != 0)
			features[k+i*dim] /= x;
	}

	public void resize(int newdim,int newsize)
	{
		if (newdim != dim)
			throw new IllegalArgumentException("Invalid dim "+dim+" != "+newdim);
		float[] newdata = new float[newdim*newsize];
		System.arraycopy(this.features,0,newdata,0,Math.min(dim*size,newdim*newsize));
		this.features = newdata;
		this.dim = newdim;	
		this.size = newsize;
	}

	public void remap(FeatureList list, int newdim,int newsize)
	{
		if (newdim*newsize != list.dim*list.size)
			throw new IllegalArgumentException("Invalid list.dim*list.size "+list.dim*list.size+" != "+newdim*newsize);
		this.features = new float[newdim*newsize];
		System.arraycopy(list.features,0,this.features,0,newdim*newsize);
		this.dim = newdim;	
		this.size = newsize;
	}

	public	int[]	compNN (FeatureList l)
	{
		int[] res = new int[size];
		for (int i=0;i<size;i++) {
			int mj = 0;
			double ms = 1E99;
			for (int j=0;j<l.size;j++) {
				double s = 0;
				for (int k=0;k<dim;k++) {
					double x = features[k+i*dim] - l.features[k+j*dim];
					s += x*x;
				}
				if (s < ms) {
					ms = s;
					mj = j; 
				}
			}
			res[i] = mj;
		}
		return res;
	}

	public	int	countMatches (FeatureList l)
	{
		int[] m1 = compNN(l);
		int[] m2 = l.compNN(this);
		int count = 0;
		for (int i=0;i<size;i++) {
			if (m2[m1[i]] == i)
				count ++;
		}
		return count;
	}

	public BytesList toBytesList () {
		return new BytesList(this);
	}

	@Override
	public boolean equals(Object o) {
		if (o == this)
			return true;
		if (!(o instanceof FeatureList))
			return false;
		FeatureList x = (FeatureList)o;
		if (x.dim != dim || x.size != size)
			return false;
		return Arrays.equals(x.features,features);
	}

	@Override
	public BufferedImage toBufferedImage () throws Exception
	{
		BufferedImage image = null;
		byte[] buffer = null;
		if (dim == 1)
		{
			image = new BufferedImage(size,1,BufferedImage.TYPE_BYTE_GRAY);
			buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			for (int i=0;i<size;i++)
			{
				int x = (int)(features[i] * 255.0f);
				if (x < 0) x = 0;
				else if (x > 255) x = 255;
				buffer[i] = (byte)(x);
			}
		}
		else if (dim == 3)
		{
			image = new BufferedImage(size,1,BufferedImage.TYPE_3BYTE_BGR);
			buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			for (int i=0;i<size;i++)
			{
				int x = (int)(features[3*i+2] * 255.0f);
				if (x < 0) x = 0;
				else if (x > 255) x = 255;
				buffer[3*i+0] = (byte)(x);

				x = (int)(features[3*i+1] * 255.0f);
				if (x < 0) x = 0;
				else if (x > 255) x = 255;
				buffer[3*i+1] = (byte)(x);

				x = (int)(features[3*i+0] * 255.0f);
				if (x < 0) x = 0;
				else if (x > 255) x = 255;
				buffer[3*i+2] = (byte)(x);
			}
		}
		else
			throw new Exception ("convertToBufferedImage() Invalid dimensions");
		return image;
	}
        
        public void pow(double power) {
            if (power == 1)
                return;
            for(int i=0 ; i<dim*size ; i++)
                if(features[i]>=0)
                    features[i] = (float) Math.pow(features[i], power);
                else
                    features[i] = (float) -Math.pow(-features[i], power);
        }
        
        public void normalize(double power, String normalize) {
            pow(power);
            if (normalize == null || normalize.equals("none"))
                return;
            float norm;
            for(int i = 0 ; i < size ; i++){
                if (normalize.equals("l2"))
                    norm = getNormL2(i);
                else
                    throw new IllegalArgumentException("Invalid norm "+normalize);
                if (Math.abs(norm) < 1E-5)
                    return;
                div(i,norm);
            }
        }
        
        public void weighted(float [] Ws){
            if(Ws.length != size)
                return;
            for(int i = 0 ; i < size ; i++){
                float w = Ws[i];
                for(int j = 0 ; j < dim ; j++)
                    features[j+i*dim] = features[j+i*dim]*w;
            }
            
        }
        
}
