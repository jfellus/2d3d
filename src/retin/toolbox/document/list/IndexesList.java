/**
 * \file IndexesList.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.list;

import retin.toolbox.document.Document;

public class IndexesList extends Document
{
	private int dim;
	private int maxb;
	private int size;
	private int[] indexes;

	public	IndexesList ()
	{
	}

	public IndexesList (int dim,int maxb,int size)
	{
		this.dim = dim;
		this.maxb = maxb;
		this.size = size;
		this.indexes = new int[dim*size];
	}

        //! Taille des index/histogrammes (=un élément de la liste).
	public int getDim () { return dim; }
        //! Nombre d'index/histogrammes
	public int getSize() { return size; }
        //! La plus grande valeur d'un bin d'index/histogramme
	public int getMaxBinValue() { return maxb; }
	public int[] getIndexes() { return indexes; }

	public void setValue (int i,int j,int x) 
	{
		indexes[i+j*dim] = x;
	}
	public int getValue (int i,int j)
	{
		return indexes[i+j*dim];
	}
	public void incValue (int i,int j) 
	{
		indexes[i+j*dim] ++;
	}

}
