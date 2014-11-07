/**
 * \file Indexes.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.basic;

import retin.toolbox.document.Document;


public class Indexes extends Document
{
	private int dim,length;
	private int[] data;

	public	Indexes ()
	{
		this.dim = 0;
		this.length = 0;
	}
	public Indexes (int dim,int length)
	{
		this.dim = dim;
		this.length = length;
		this.data = new int[length];
	}
	public Indexes (int dim,int[] data)
	{
		this.dim = dim;
		this.length = data.length;
		this.data = new int[dim];
		System.arraycopy (data,0,this.data,0,dim);
	}

	public int getDim () { return dim; }
	public int getLength () { return length; }
	public int[] getData() { return data; }

	public int getValue(int i) { return data[i]; }
	public void setValue(int i,int x) { data[i] = x; }
	void incValue(int i) { data[i]++; }

	public int getArgMax() {
		int m = 0;
		for (int i=1;i<data.length;i++) {
			if (data[i] > data[m]) m = i;
		}
		return m;
	}

	public void show () 
	{
		for (int i=0;i<length;i++)
			System.out.print(data[i]+", ");
		System.out.println(); 
	}


}
