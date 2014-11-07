/**
 * \file FeatureMatrix.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.util.Arrays;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.list.FeatureList;

public class FeatureMatrix extends Document
{
	protected int dim;
	protected int width,height;
	protected float[] features;

	public	FeatureMatrix ()
	{
		this.width = 0;
		this.height = 0;
		this.dim = 0;
	}
	public FeatureMatrix (int width,int height,int dim)
	{
		this.width = width;
		this.height = height;
		this.dim = dim;
		this.features = new float[dim*width*height];
	}
	public FeatureMatrix(BytesMatrix m)
	{
		this.width = m.getWidth();
		this.height = m.getHeight();
		this.dim = m.getDim();
		this.features = new float[dim*width*height];
		byte[] bytes = m.getBytes();
		for (int i=0;i<bytes.length;i++)
			features[i] = ((int)bytes[i]) & 0xFF;
	}
	public FeatureMatrix (BufferedImage image) throws Exception
	{
		width = image.getWidth();
		height = image.getHeight();
		int n = width*height;

		if (image.getType() == BufferedImage.TYPE_3BYTE_BGR)
		{
			dim = 3;
			features = new float[n*dim];
			byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			for (int i=0;i<n;i++)
			{
				features[3*i+0] = (((int)buffer[3*i+2]) & 0xFF) / 255.0f;
				features[3*i+1] = (((int)buffer[3*i+1]) & 0xFF) / 255.0f;
				features[3*i+2] = (((int)buffer[3*i+0]) & 0xFF) / 255.0f;
			}
		}
		else if (image.getType() == BufferedImage.TYPE_BYTE_GRAY)
		{
			dim = 1;
			features = new float[n*dim];
			byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			for (int i=0;i<n;i++)
				features[i] = (((int)buffer[i]) & 0xFF) / 255.0f;
		}
		else if (image.getType() == BufferedImage.TYPE_CUSTOM)
		{
			//System.out.println(" *** Warning *** Custom image file format");
			dim = 3;
			features = new float[n*dim];
			byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			if (buffer.length != features.length)
				throw new Exception ("FeatureMatrix(BufferedImage) Unsupported custom format");
			for (int i=0;i<3*n;i++)
			{
				features[i] = (((int)buffer[i]) & 0xFF) / 255.0f;
			}
		}
		else
			throw new Exception ("FeatureMatrix(BufferedImage) Unsupported format "+image);
	}

	public int getDim () { return dim; }
	public int getWidth() { return width; }
	public int getHeight() { return height; }
	public float[] getFeatures() { return features; }

	public Feature getFeature(int i,int j) {
		Feature f = new Feature(dim);
		System.arraycopy(features,dim*(i+j*width),f.getData(),0,dim);
		return f;
	}
	public void setFeature(int i,int j,Feature f) {
		System.arraycopy(f.getData(),0,features,dim*(i+j*width),dim);
	}

	public void setValue (int i,int j,int k,float x) 
	{
		features[k+dim*(i+j*width)] = x;
	}
	public float getValue (int i,int j,int k) 
	{
		return features[k+dim*(i+j*width)];
	}

	public void setRow(int r,FeatureList list)
	{
		if (list.getDim() != dim)
			throw new IllegalArgumentException("Invalid dim "+dim+" != "+list.getDim());
		if (list.getSize() != width)
			throw new IllegalArgumentException("Invalid size "+width+" != "+list.getSize());
		if (r < 0 || r >= height)
			throw new IllegalArgumentException("Invalid row "+r);
		System.arraycopy(list.getFeatures(), 0, features, r*dim*width, dim*width);
	}

	public	void add (float x)
	{
		for (int i=0;i<features.length;i++)
			features[i] += x;
	}
	public	void mul (float x)
	{
		for (int i=0;i<features.length;i++)
			features[i] *= x;
	}

	public	FeatureMatrix getSubFeature (int k)
	{
		FeatureMatrix m = new FeatureMatrix(width,height,1);
		for (int i=0;i<width*height;i++)
			m.features[i] = features[k+i*dim];
		return m;
	}
	public	FeatureMatrix getSubFeatures (int k1,int k2)
	{
		int d = k2-k1+1;
		if (d <= 0) return null;
		FeatureMatrix m = new FeatureMatrix(width,height,d);
		for (int i=0;i<width*height;i++)
			for (int k=0;k<d;k++)
				m.features[k+i*d] = features[k1+k+i*dim];
		return m;
	}
	public	FeatureMatrix	getFeatureNormL2 ()
	{
		FeatureMatrix m = new FeatureMatrix(width,height,1);
		for (int i=0;i<width*height;i++) {
			float w = 0;
			for (int k=0;k<dim;k++) {
				double x = features[k+i*dim];
				w += x*x;
			}
			m.features[i] = (float)Math.sqrt(w);
		}
		return m;
	}
	
	public	float	minValue()
	{
		float w = features[0];
		for (int i=0;i<width*height;i++) {
			float d = features[i];
			if (d < w) w = d;
		}
		return w;
	}
	public	float	maxValue()
	{
		float w = features[0];
		for (int i=1;i<width*height;i++) {
			float d = features[i];
			if (d > w) w = d;
		}
		return w;
	}
	
	public	FeatureMatrix	normalizeMax ()
	{
		mul (1.0f / maxValue());
		return this;
	}

	public FeatureMatrix normalizeMinMax() {
		float min = minValue();
		float max = maxValue();
		float range = max - min;
		if (range < 1E-7f) range = 1E-7f;
		for (int i=0;i<width*height;i++)
			features[i] = (features[i] - min) / range;
		return this;
	}
	
	public	double	distanceL2(FeatureMatrix x)
	{
		double w = 0;
		for (int i=0;i<features.length;i++) {
			double d = features[i] - x.features[i];
			w += d*d;
		}
		return Math.sqrt(w);
	}
	public	double	distanceMax(FeatureMatrix x)
	{
		double w = 0;
		for (int i=0;i<features.length;i++) {
			double d = Math.abs(features[i] - x.features[i]);
			if (d > w) w = d;
		}
		return w;
	}

	@Override
	public	boolean equals(Object o)
	{
		if (o == this)
			return true;
		if (!(o instanceof FeatureMatrix))
			return false;
		FeatureMatrix x = (FeatureMatrix)o;
		if (x.dim != dim || x.width != width || x.height != height)
			return false;
		return Arrays.equals(x.features,features);
	}

	@Override
	public	Object 	convertTo (Class classObject) throws Exception
	{
		if (classObject.equals(getClass()))
			return this;
		else if (classObject.equals(FeatureList.class))
			return new FeatureList(this);
		else if (classObject.equals(BufferedImage.class))
			return toBufferedImage();
		throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
	}
	@Override
	public BufferedImage toBufferedImage () throws Exception
	{
		BufferedImage image = null;
		byte[] buffer = null;
		if (dim == 1)
		{
			image = new BufferedImage(width,height,BufferedImage.TYPE_BYTE_GRAY);
			buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			int n = width*height;
			for (int i=0;i<n;i++)
			{
//				if (i > n-100) System.out.println(features[i]);
				int x = (int)(features[i] * 255.0f);
				if (x < 0) x = 0;
				else if (x > 255) x = 255;
				buffer[i] = (byte)(x);
			}
		}
		else if (dim == 3)
		{
			image = new BufferedImage(width,height,BufferedImage.TYPE_3BYTE_BGR);
			buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			int n = width*height;
			for (int i=0;i<n;i++)
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
}
