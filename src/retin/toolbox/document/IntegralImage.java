 /**
 * \file IntegralImage.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import retin.toolbox.document.basic.Feature;

public abstract class IntegralImage extends Document
{
	public abstract int getDim ();
	public abstract int getWidth();
	public abstract int getHeight();
	public abstract float getValue(int i,int j,int k);

	public float getCount(int i1,int j1,int n,int m,int k) {

		float result = getValue(i1+n,j1+m,k);
		result -= getValue(i1,j1+m,k);
		result -= getValue(i1+n,j1,k);
		result += getValue(i1,j1,k);
		return result;
	}

	public Feature	getHistogram(int i1,int j1,int n,int m) {
		if (n > getWidth()) n = getWidth();
		if (m > getHeight()) m = getHeight();
		Feature h = new Feature(getDim());
		for (int k=0;k<getDim();k++)
			h.setValue(k,getCount(i1,j1,n,m,k));
		return h;
	}
	public Feature	getHistogram() {
		return getHistogram(0,0,this.getWidth(),this.getHeight());
	}

	public	void	showErrors (IndexMatrix map) {
		int width = getWidth();
		int height = getHeight();
		int dim = getDim();
		FloatMatrix ii = new FloatMatrix(width,height);
		float minError = 1E19f,aveError = 0,maxError = -1E19f;
		for (int k=0;k<dim;k++) {
			for (int j=0;j<height;j++) {
				for (int i=0;i<width;i++) {
					float value = ((map.getValue(i,j)==k)?1:0);
					if (j>0) value += ii.getValue(i,j-1);
					if (i>0) value += ii.getValue(i-1,j);
					if (i>0 && j>0) value -= ii.getValue(i-1,j-1);
					ii.setValue(i,j,value);
					float error = Math.abs(value-getValue(i+1,j+1,k));
					aveError += error;
					if (error < minError) minError = error;
					if (error > maxError) maxError = error;
					/*if ( (i%4) == 0 && (j%4) == 0 && error > 0) {
						System.out.println(i+","+j+","+k+" = "+((map.getValue(i,j)==k)?1:0)+" ii="+value+" getvalue="+getValue(i,j,k));
						//return;
					}*/
				}
			}
		}
		aveError /= dim*width*height;
		System.out.println(String.format("Error : %.2f < %.2f < %.2f",minError,aveError,maxError));

	}

}

 
