 /**
 * \file GlobalLabel.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import retin.toolbox.document.Document;

public class GlobalLabel extends Document implements Cloneable
{
	protected int	label;
	protected float weight;

	public	GlobalLabel()
	{
		label = 0;
		weight = 1;
	}
	public	GlobalLabel(int label,float weight)
	{
		this.label = label;
		this.weight = weight;
	}
	public	GlobalLabel(GlobalLabel g)
	{
		this.label = g.label;
		this.weight = g.weight;
	}

	public	int	getLabel() { return label; }
	public	void	setLabel(int label) { this.label = label; }
	public  float	getWeight() { return weight; }
	public	void	setWeight(float weight) { this.weight = weight; }

	@Override
	public	boolean	equals(Object o) {
	        if (o == null)
			return false;
		if (o == this)
			return true;
		if (!(o instanceof GlobalLabel))
			return false;
		GlobalLabel x = (GlobalLabel)o;
		return x.label == label && Math.abs(x.weight-weight) < 1E-5;
	}
	@Override 
	public	Object clone() {
		return new GlobalLabel(this);
	}
        @Override
        public  String  toString() {
            return label+":"+weight;
        }
}

	
