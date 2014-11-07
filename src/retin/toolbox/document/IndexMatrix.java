/**
 * \file IndexMatrix.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.awt.image.*;
import retin.toolbox.document.Document;
import retin.toolbox.document.basic.Feature;


public class IndexMatrix extends Document
{
	private int dim;
	private int width,height;
	private int[] indexes;

	public	IndexMatrix ()
	{
		this.width = 0;
		this.height = 0;
		this.dim = 0;
	}

	public IndexMatrix (int width,int height)
	{
		this.dim = 0;
		this.width = width;
		this.height = height;
		this.indexes = new int[width*height];
	}

	public IndexMatrix (int width,int height, int dim)
	{
		this.dim = dim;
		this.width = width;
		this.height = height;
		this.indexes = new int[width*height];
	}

	public void setDim(int x) { dim = x; }

	public int getDim () { return dim; }
	public int getWidth() { return width; }
	public int getHeight() { return height; }
	public int[] getIndexes() { return indexes; }

	public void setValue (int i,int j,int x) 
	{
		indexes[i+j*width] = x;
	}
	public int getValue (int i,int j)
	{
		return indexes[i+j*width];
	}
	public void incValue (int i,int j) 
	{
		indexes[i+j*width] ++;
	}

	public int findDim()
	{
		dim = 0;
		for (int i=0;i<indexes.length;i++)
			if (indexes[i] > dim)
				dim = indexes[i];
		dim ++;
		return dim;
	}	

	public	int	distanceMax(IndexMatrix x)
	{
		int w = 0;
		for (int i=0;i<indexes.length;i++) {
			int d = indexes[i] - x.indexes[i];
			if (d < 0) d = -d;
			if (d > w) w = d;
		}
		return w;
	}

	public Feature	getHistogram(int i1,int j1,int n,int m) {
		if (n > width) n = width;
		if (m > height) m = height;
		Feature h = new Feature(dim);
		for (int j=j1;j<j1+m;j++)
			for (int i=i1;i<i1+n;i++) {
				h.incValue(indexes[i+j*width]);
			}
		return h;
	}
	public Feature getHistogram() {
		return getHistogram(0,0,width,height);
	}

	public IndexMatrix getSubIndexMatrix (int iFirst, int iNb, int jFirst, int jNb)
	{
		if (iFirst>=this.width || jFirst>=this.height)
			return new IndexMatrix();
		IndexMatrix subIM = new IndexMatrix(iNb,jNb,this.dim);
		for (int h=0; h<jNb; h++)
			System.arraycopy(this.indexes, iFirst+(jFirst+h)*this.width, subIM.getIndexes(), h*iNb,iNb);
		return subIM;
	}

	@Override
	public BufferedImage toBufferedImage () throws Exception
	{
		BufferedImage image = new BufferedImage(width,height,BufferedImage.TYPE_BYTE_GRAY);
		byte[] buffer = ((DataBufferByte)image.getRaster().getDataBuffer()).getData();
		int n = width*height;
		for (int i=0;i<n;i++)
		{
			int x = (indexes[i]*255)/dim;
			if (x < 0) x = 0;
			else if (x > 255) x = 255;
			buffer[i] = (byte)(x);
		}
		return image;
	
	}
}
