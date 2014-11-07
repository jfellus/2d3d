/**
 * \file Filter.java
 * \author Romain Negrel
 * \version 4.0
 */

package retin.toolbox.document.list;

import retin.toolbox.document.Document;

public class FiltersList extends Document
{
	private int width,height;
        private int filtersCount;
	private float[] data;

	public	FiltersList ()
	{
		this.width = 0;
		this.height = 0;
                this.filtersCount = 0;
	}
        
        public FiltersList (float[] data, int filtersCount, int width,int height) throws Exception
	{
                if(data.length != width*height*filtersCount)
                    throw new Exception("errer size");
		this.width = width;
		this.height = height;
                this.filtersCount = filtersCount;
		this.data = data;
	}
        
	public FiltersList (int filtersCount, int width,int height)
	{
		this.width = width;
		this.height = height;
                this.filtersCount = filtersCount;
		this.data = new float[width*height];
	}
        
        public void resize (int filtersCount, int width,int height) {
		if (this.filtersCount == filtersCount && this.width == width && this.height == height)
			return;
		float[] v = new float[width*height*filtersCount];
		
		this.width = width;
		this.height = height;
                this.filtersCount = filtersCount;
		this.data = v;
	}

	public int getWidth() { return width; }
	public int getHeight() { return height; }
        public int getFiltersCount() { return this.filtersCount; }
	public float[] getData() { return data; }

	public void setValue (int id, int i,int j,float x) 
	{
		data[i+j*width+id*width*height] = x;
	}
	
	public float getValue (int id, int i,int j) 
	{
		return data[i+j*width+id*width*height];
	}

    @Override
	public	Object 	convertTo (Class classObject) throws Exception
	{
		if (classObject.equals(getClass()))
			return this;
		throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
	}
}
