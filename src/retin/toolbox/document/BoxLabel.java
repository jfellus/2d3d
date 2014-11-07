 /**
 * \file BoxLabel.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

public class BoxLabel extends GlobalLabel implements Cloneable
{
	float	x1,y1,x2,y2;

	public	BoxLabel(float x1,float y1,float x2,float y2)
	{
		super();
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
	}
	public	BoxLabel(float x1,float y1,float x2,float y2,int label,float weight)
	{
		super(label,weight);
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
	}
	public	BoxLabel(BoxLabel b)
	{
		super(b.label,b.weight);
		this.x1 = b.x1;
		this.y1 = b.y1;
		this.x2 = b.x2;
		this.y2 = b.y2;
	}
	public	BoxLabel(double x1,double y1,double x2,double y2)
	{
		super();
		this.x1 = (float)x1;
		this.y1 = (float)y1;
		this.x2 = (float)x2;
		this.y2 = (float)y2;
	}

	public	float	getWidth() { return x2-x1; }
	public	float	getHeight() { return y2-y1; }
	public	float	getX1 () { return x1; }
	public	float	getY1 () { return y1; }
	public	float	getX2 () { return x2; }
	public	float	getY2 () { return y2; }

	public	void	setX1 (float x) { x1 = x; }
	public	void	setY1 (float x) { y1 = x; }
	public	void	setX2 (float x) { x2 = x; }
	public	void	setY2 (float x) { y2 = x; }

	public	float	area() {
		return Math.abs(x2-x1)*Math.abs(y2-y1);
	}

	public	void	swapPoints()
	{
		if (x1 > x2)
		{
			float x = x1;
			x1 = x2;
			x2 = x;
		}
		if (y1 > y2)
		{
			float y = y1;
			y1 = y2;
			y2 = y;
		}
	}

	@Override
	public	void	show() {
		System.out.println("("+x1+","+y1+" ; "+x2+","+y2+") label="+label+", weight="+weight);
	}

	@Override
	public	boolean	equals(Object o) {
		if (o == null)
			return false;
		if (o == this)
			return true;
		if (!(o instanceof BoxLabel))
			return false;
		BoxLabel x = (BoxLabel)o;
		return x.label == label && Math.abs(x.weight-weight) < 1E-5
		    && Math.abs(x.x1-x1) < 1E-5 && Math.abs(x.y1-y1) < 1E-5 
		    && Math.abs(x.x2-x2) < 1E-5 && Math.abs(x.y2-y2) < 1E-5;
	}
	@Override 
	public	Object clone() {
		return new BoxLabel(this);
	}

}

 
