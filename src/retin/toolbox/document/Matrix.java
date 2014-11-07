/**
 * \file Matrix.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import retin.toolbox.document.Document;

public class Matrix extends Document
{
	private int dim;
	private int vectorSize,vectorCount;
	private double[] vectors;

	public	Matrix ()
	{
		this.vectorSize = 0;
		this.vectorCount = 0;
	}
	public Matrix (int vectorSize,int vectorCount)
	{
		this.vectorSize = vectorSize;
		this.vectorCount = vectorCount;
		this.vectors = new double[vectorSize*vectorCount];
	}
        
        public Matrix (FloatMatrix matF)
	{
		vectorSize = matF.getVectorSize();
		vectorCount = matF.getVectorCount();
		vectors = new double[vectorSize*vectorCount];
                float[] vectorsF = matF.getData();
                for(int i = 0 ; i < vectorSize*vectorCount; i++){
                    vectors[i] = (double) vectorsF[i];
                }
	}

	public int getVectorSize() { return vectorSize; }
	public int getVectorCount() { return vectorCount; }
	public double[] getData() { return vectors; }

	public void setValue (int i,int j,double x) 
	{
		vectors[i+j*vectorSize] = x;
	}

	public double getValue (int i,int j) 
	{
		return vectors[i+j*vectorSize];
	}

	public	double	meanValue()
	{
		double sum = 0;
		for (int i=0;i<vectorCount*vectorSize;i++)
			sum += vectors[i];
		return sum / ((double)(vectorCount*vectorSize));
	}

	public	Object 	convertTo (Class classObject) throws Exception
	{
		if (classObject.equals(getClass()))
			return this;
		throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
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
