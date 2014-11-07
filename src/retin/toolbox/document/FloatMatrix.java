/**
 * \file FloatMatrix.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import retin.toolbox.document.Document;

public class FloatMatrix extends Document
{
	private int dim;
	private int vectorSize,vectorCount;
	private float[] vectors;

	public	FloatMatrix ()
	{
		this.vectorSize = 0;
		this.vectorCount = 0;
	}
        
        public FloatMatrix (float[] vectors, int vectorSize,int vectorCount) throws Exception
	{
                if(vectors.length != vectorSize*vectorCount)
                    throw new Exception("errer size");
		this.vectorSize = vectorSize;
		this.vectorCount = vectorCount;
		this.vectors = vectors;
	}
        
	public FloatMatrix (int vectorSize,int vectorCount)
	{
		this.vectorSize = vectorSize;
		this.vectorCount = vectorCount;
		this.vectors = new float[vectorSize*vectorCount];
	}

	public int getVectorSize() { return vectorSize; }
	public int getVectorCount() { return vectorCount; }
	public float[] getData() { return vectors; }

	public void resize (int vectorSize,int vectorCount) {
		if (this.vectorSize == vectorSize && this.vectorCount == vectorCount)
			return;
		float[] v = new float[vectorSize*vectorCount];
		int m = Math.min(vectorCount,this.vectorCount);
		int n = Math.min(vectorSize,this.vectorSize);
		for (int j=0;j<m;j++)
			for(int i=0;i<n;i++)
				v[i+j*vectorSize] = this.vectors[i+j*this.vectorSize];
		this.vectorSize = vectorSize;
		this.vectorCount = vectorCount;
		this.vectors = v;
	}
	public void setMinSize(int vectorSize,int vectorCount) {
		resize(Math.max(this.vectorSize,vectorSize),Math.max(this.vectorCount,vectorCount));
	}

	public void setValue (int i,int j,float x) 
	{
		vectors[i+j*vectorSize] = x;
	}
	public void incValue (int i,int j) 
	{
		vectors[i+j*vectorSize] ++;
	}
	public float getValue (int i,int j) 
	{
		return vectors[i+j*vectorSize];
	}

	public float[] getColumn (int j)
	{
		float[] result = new float[vectorSize];
		System.arraycopy(vectors,j*vectorSize,result,0,vectorSize);
		return result;
	}
        public void setColumn(int j,float[] data)
        {
		System.arraycopy(data,0,vectors,j*vectorSize,vectorSize);
        }

	public void mul (float x) 
	{
		for (int i=0;i<vectorCount*vectorSize;i++)
			vectors[i] *= x;
	}
	public void div (float x) 
	{
		mul (1.0f / x);
	}

	public	float	maxValue()
	{
		float m = -1E29f;
		for (int i=0;i<vectorCount*vectorSize;i++) {
			if (vectors[i] > m) m = vectors[i];
		}
		return m;
	}
	public	float	meanValue()
	{
		float sum = 0;
		for (int i=0;i<vectorCount*vectorSize;i++)
			sum += vectors[i];
		return sum / ((float)(vectorCount*vectorSize));
	}
	public	float[]	meanRow() {
		float[] r = new float[vectorCount];
		for (int j=0;j<vectorCount;j++) {
			for (int i=0;i<vectorSize;i++)
				r[j] += vectors[i+j*vectorSize];
			r[j] /= vectorSize;
		}
		return r;
	}
	public	float[]	meanCol() {
		float[] r = new float[vectorSize];
		for (int j=0;j<vectorCount;j++)
			for (int i=0;i<vectorSize;i++)
				r[i] += vectors[i+j*vectorSize];
		for (int i=0;i<vectorSize;i++)
			r[i] /= vectorCount;
		return r;
	}


	public	Object 	convertTo (Class classObject) throws Exception
	{
		if (classObject.equals(getClass()))
			return this;
		throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
	}
	@Override
	public BufferedImage toBufferedImage () throws Exception
	{
		BufferedImage image = null;
		byte[] buffer = null;
		image = new BufferedImage(vectorSize,vectorCount,BufferedImage.TYPE_BYTE_GRAY);
		buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
		int n = vectorSize*vectorCount;
		for (int i=0;i<n;i++)
		{
//				if (i > n-100) System.out.println(features[i]);
			int x = (int)(vectors[i] * 255.0f);
			if (x < 0) x = 0;
			else if (x > 255) x = 255;
			buffer[i] = (byte)(x);
		}
		return image;
	}

	public	void	addAlpha (BufferedImage image) {
		if (image.getWidth() != vectorSize)
			throw new java.lang.IllegalArgumentException("width");
		if (image.getHeight() != vectorCount)
			throw new java.lang.IllegalArgumentException("height");
		float m = maxValue();
		for (int j=0;j<image.getHeight();j++) {
			for (int i=0;i<image.getWidth();i++) {
				float w = vectors[i+j*vectorSize] / m;
				int rgb = image.getRGB(i,j);
				int r = (int) ((rgb&0xFF) * w);
				int g = (int) (((rgb&0xFF00) >> 8) * w);
				int b = (int) (((rgb&0xFF0000) >> 16) * w);
				image.setRGB(i,j, r + (g<<8) + (b<<16) );
			}
		}
	}

	public	void	show (int i0,int j0,int i1,int j1)
	{
		for (int j=j0;j<j1;j++)
		{
			for (int i=i0;i<i1;i++)
				System.out.print(" "+getValue(i,j));
			System.out.println ();
		}
	}
}
