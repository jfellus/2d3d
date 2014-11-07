/**
 * \file Keypoint.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.basic;

import java.awt.*;
import java.awt.image.BufferedImage;
import retin.toolbox.document.Document;

public class Keypoint extends Document
{
	private int	dim;
	private float	score;
	private float[] coord;
	private float[] size;
	private float[] dir;

	public	Keypoint () {
	}

	public  Keypoint (float x,float y,float sx,float sy,float dx,float dy,float score) {
		dim = 2;
		this.score = score;
		coord = new float[2];
		coord[0] = x;
		coord[1] = y;
		size = new float[2];
		size[0] = sx;
		size[1] = sy;
		dir = new float[2];
		dir[0] = dx;
		dir[1] = dy;
	}
	public	float	getScore() {
		return score;
	}
	public float getX() {
		return coord[0];
	}
	public float getY() {
		return coord[1];
	}
	public float getSizeX() {
		return size[0];
	}
	public float getSizeY() {
		return size[1];
	}
	public float getDirX() {
		return dir[0];
	}
	public float getDirY() {
		return dir[1];
	}

	public	void	draw (Graphics2D g,float scalex,float scaley)
	{
		int x = (int)(coord[0]);
		int y = (int)(coord[1]);
		g.setColor(Color.red);
		g.drawLine(x-2,y,x+2,y);
		g.drawLine(x,y-2,x,y+2);
		g.drawLine(x,y,(int)(x+size[0]*dir[0]*scalex),(int)(y+size[0]*dir[1]*scaley));
		g.setColor(Color.blue);
		drawEllipse (g,coord[0],coord[1],scalex,scaley);
		g.setColor(Color.white);
		drawEllipse (g,coord[0],coord[1],scalex,scaley);
	}

	public	void	drawEllipse (Graphics2D g,float x,float y,float scalex,float scaley)
	{
		double w = size[0];
		double h = size[1];
		double x1=0,y1=0;
		for (double t=0;t<2*Math.PI;t+=Math.PI/50) {
			double x0b = w * Math.cos(t);
			double y0b = h * Math.sin(t);
			double x0 = x - (x0b*dir[0] + y0b*dir[1])*scalex;
			double y0 = y + (y0b*dir[0] - x0b*dir[1])*scaley;
			if (t > 0) {
				g.drawLine((int)x1,(int)y1,(int)x0,(int)y0);
			}
			x1 = x0;
			y1 = y0;
		}
	}

	@Override
	public	void	show () 
	{
		System.out.println(
			"score="+score
			+",x="+coord[0]+",y="+coord[1]
			+",sx="+size[0]+",sy="+size[1]
			+",dx="+dir[0]+",dy="+dir[1]
		);
	}

}
