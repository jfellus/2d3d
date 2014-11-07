/**
 * \file Filter.java
 * \author Romain Negrel
 * \version 4.0
 */

package retin.toolbox.document;

public class Filter extends Document
{
	private int width,height;
	private float[] vectors;

	public	Filter ()
	{
		this.width = 0;
		this.height = 0;
	}
        
        public Filter (float[] vectors, int width,int height) throws Exception
	{
                if(vectors.length != width*height)
                    throw new Exception("errer size");
		this.width = width;
		this.height = height;
		this.vectors = vectors;
	}
        
	public Filter (int width,int height)
	{
		this.width = width;
		this.height = height;
		this.vectors = new float[width*height];
	}
        
        public void resize (int width,int height) {
		if (this.width == width && this.height == height)
			return;
		float[] v = new float[width*height];
		
		this.width = width;
		this.height = height;
		this.vectors = v;
	}

	public int getWidth() { return width; }
	public int getHeight() { return height; }
	public float[] getData() { return vectors; }

	public void setValue (int i,int j,float x) 
	{
		vectors[i+j*width] = x;
	}
	
	public float getValue (int i,int j) 
	{
		return vectors[i+j*width];
	}

    @Override
	public	Object 	convertTo (Class classObject) throws Exception
	{
		if (classObject.equals(getClass()))
			return this;
		throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
	}
}
