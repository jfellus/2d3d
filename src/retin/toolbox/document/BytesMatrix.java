/**
 * \file BytesMatrix.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.awt.image.*;

public class BytesMatrix extends Document
{
	private int dim;
	private int width,height;
	private byte[] bytes;

	public	BytesMatrix ()
	{
		this.width = 0;
		this.height = 0;
		this.dim = 0;
	}
	public BytesMatrix (int width,int height,int dim)
	{
		this.width = width;
		this.height = height;
		this.dim = dim;
		this.bytes = new byte[dim*width*height];
	}
	public BytesMatrix (BufferedImage image) throws Exception
	{
		width = image.getWidth();
		height = image.getHeight();
		int n = width*height;

		if (image.getType() == BufferedImage.TYPE_3BYTE_BGR)
		{
			dim = 3;
			bytes = new byte[n*dim];
			byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			for (int i=0;i<n;i++)
			{
				bytes[3*i+0] = buffer[3*i+2];
				bytes[3*i+1] = buffer[3*i+1];
				bytes[3*i+2] = buffer[3*i+0];
			}
		}
                else if (image.getType() == BufferedImage.TYPE_BYTE_GRAY)
                {
                    dim = 1;
                    bytes = new byte[n];
                    byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
                    System.arraycopy(buffer,0,bytes,0,n);
                }
                else if (image.getType() == BufferedImage.TYPE_CUSTOM)
		{
			System.out.println(" *** Warning *** Custom image file format");
			dim = 3;
			bytes = new byte[n*dim];
			byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			if (buffer.length != bytes.length)
				throw new Exception ("BytesMatrix(BufferedImage) Unsupported custom format");
	                System.arraycopy(buffer,0,bytes,0,n*dim);
		}
		else
			throw new Exception ("BytesMatrix(BufferedImage) Unsupported format");
	}

	public int getDim () { return dim; }
	public int getWidth() { return width; }
	public int getHeight() { return height; }
	public byte[] getBytes() { return bytes; }

	public void setValue (int i,int j,int k,byte x) 
	{
		bytes[k+dim*(i+j*width)] = x;
	}
	public byte getValue (int i,int j,int k) 
	{
		return bytes[k+dim*(i+j*width)];
	}

	public	BytesMatrix getSubFeature (int k)
	{
		BytesMatrix m = new BytesMatrix(width,height,1);
		for (int i=0;i<width*height;i++)
			m.bytes[i] = bytes[k+i*dim];
		return m;
	}
	public	BytesMatrix getSubFeatures (int k1,int k2)
	{
		int d = k2-k1+1;
		if (d <= 0) return null;
		BytesMatrix m = new BytesMatrix(width,height,d);
		for (int i=0;i<width*height;i++)
			for (int k=0;k<d;k++)
				m.bytes[k+i*d] = bytes[k1+k+i*dim];
		return m;
	}
	
	public	double	distanceL2(BytesMatrix x)
	{
		double w = 0;
		for (int i=0;i<width*height;i++) {
			double d = bytes[i] - x.bytes[i];
			w += d*d;
		}
		return Math.sqrt(w);
	}
	public	double	distanceMax(BytesMatrix x)
	{
		double w = 0;
		for (int i=0;i<width*height;i++) {
			double a = ((int)bytes[i]) & 0xFF;
			double b = ((int)x.bytes[i]) & 0xFF;
			double d = Math.abs(a-b);
			if (d > w) w = d; 
		}
		return w;
	}

	@Override
	public	Object 	convertTo (Class classObject) throws Exception
	{
		if (classObject.equals(BufferedImage.class))
			return toBufferedImage();
		throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
	}
	public BufferedImage toBufferedImage () throws Exception
	{
		BufferedImage image = null;
		byte[] buffer = null;
		if (dim == 1)
		{
			image = new BufferedImage(width,height,BufferedImage.TYPE_BYTE_GRAY);
			buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();

			int n = width*height;
			int max = 1;
			for (int i=0;i<n;i++) {
				if (bytes[i] > max)
					max = bytes[i];
			}
			for (int i=0;i<n;i++) {
				int x = bytes[i];
				x = (x*255)/max;
				if (x < 0) x = 0;
				else if (x > 255) x = 255;
				buffer[i] = (byte)(x);
			}
			//System.arraycopy (bytes,0,buffer,0,width*height);
		}
		else if (dim == 3)
		{
			image = new BufferedImage(width,height,BufferedImage.TYPE_3BYTE_BGR);
			buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
			int n = width*height;
			for (int i=0;i<n;i++)
			{
				buffer[3*i+0] = bytes[3*i+2];
				buffer[3*i+1] = bytes[3*i+1];
				buffer[3*i+2] = bytes[3*i+0];
			}
		}
		else
			throw new Exception ("convertToBufferedImage() Invalid dimensions");
		return image;
	}
}
