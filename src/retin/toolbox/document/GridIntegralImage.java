 /**
 * \file GridIntegralImage.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import retin.toolbox.document.basic.Feature;

public class GridIntegralImage extends IntegralImage
{
	protected int dim;
	protected int imageWidth,imageHeight;
	protected int gridWidth,gridHeight;
	protected int	xSampling,ySampling;
	protected float[] counts;

	public GridIntegralImage (IndexMatrix map,int xSampling,int ySampling) {

		if (xSampling < 1) xSampling = 1;
		if (ySampling < 1) ySampling = 1;
		if (xSampling > map.getWidth()) xSampling = map.getWidth();
		if (ySampling > map.getHeight()) ySampling = map.getHeight();

		this.xSampling = xSampling;
		this.ySampling = ySampling;
		this.dim = map.getDim();
		this.imageWidth = map.getWidth();
		this.imageHeight = map.getHeight();

		this.gridWidth = map.getWidth() / xSampling + 1 + ((map.getWidth()%xSampling!=0)?1:0);
		this.gridHeight = map.getHeight() / ySampling + 1 +((map.getHeight()%ySampling!=0)?1:0);
		this.counts = new float[dim*gridWidth*gridHeight];

		for (int k=0;k<dim;k++) {
/*			// Première valeur
			counts[k] = (map.getValue(0,0)==k)?1:0;
			// Première ligne
			for (int i=1;i<gridWidth;i++) {
				float value = counts[k+dim*(i-1)];
				int x = (i-1)*xSampling+1;
				for (int r=0;r<xSampling;r++) {
					if (x < imageWidth)
						value += ((map.getValue(x,0)==k)?1:0);
					x ++;
				}
				counts[k+dim*i] = value;
			}
			// Première colonne
			for (int j=1;j<gridHeight;j++) {
				float value = counts[k+dim*((j-1)*gridWidth)];
				int y = (j-1)*ySampling+1;
				for (int s=0;s<ySampling;s++) {
					if (y < imageHeight)
						value += ((map.getValue(0,y)==k)?1:0);
					y ++;
				}
				counts[k+dim*(j*gridWidth)] = value;
			}
			// Centre*/
			for (int j=1;j<gridHeight;j++) {
				for (int i=1;i<gridWidth;i++) {
					float value = 0;
					int y = (j-1)*ySampling;
					for (int s=0;s<ySampling;s++) {
						int x = (i-1)*xSampling;
						for (int r=0;r<xSampling;r++) {
							if (x < imageWidth && y < imageHeight)
								value += ((map.getValue(x,y)==k)?1:0);
							x ++;
						}
						y ++;
					}
					value += counts[k+dim*(i+(j-1)*gridWidth)];
					value += counts[k+dim*((i-1)+j*gridWidth)];
					value -= counts[k+dim*((i-1)+(j-1)*gridWidth)];
					counts[k+dim*(i+j*gridWidth)] = value;
				}
			}
		}
	}

	public float getXSampling() { return xSampling; }
	public float getYSampling() { return ySampling; }
	public int getGridWidth() { return gridWidth; }
	public int getGridHeight() { return gridHeight; }

	public int getDim () { return dim; }
	public int getWidth() { return imageWidth; }
	public int getHeight() { return imageHeight; }
	public float getValue (int i,int j,int k)
	{
		// Coordonnées de la case dans la grille sous échantillonnée
		int si = i/xSampling;
		int sj = j/ySampling;

/*		if ((si+1) >= gridWidth)
			throw new IllegalArgumentException("si="+si+",width="+gridWidth);
		if ((sj+1) >= gridHeight)
			throw new IllegalArgumentException("j="+j);*/

		// Coordonées relative au bord haut gauche de la case
		float x = i%xSampling; x /= xSampling;
		float y = j%ySampling; y /= ySampling;

		float value = counts[k+dim*(si+sj*gridWidth)] * (1-x) * (1-y);
		if ((si+1) < gridWidth)
			value += counts[k+dim*((si+1)+sj*gridWidth)] * x * (1-y);
		if ((sj+1) < gridHeight)
			value += counts[k+dim*(si+(sj+1)*gridWidth)] * (1-x) * y;
		if ((si+1) < gridWidth && (sj+1) < gridHeight)
			value += counts[k+dim*((si+1)+(sj+1)*gridWidth)] * x * y;
		return value;
	}

	public native float searchCount (int width,int height,int bin);
	public native BoxLabel searchBox (int width,int height,int bin);
	public native FeatureMatrix	countMatches (Feature histogram,int n,int m);

}

 
