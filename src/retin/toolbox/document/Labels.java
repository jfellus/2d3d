 /**
 * \file Labels.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.util.ArrayList;
import java.util.AbstractList;
import retin.toolbox.document.Document;

public class Labels extends Document implements Cloneable
{
	protected AbstractList<Object> labels;

	public	static Labels	makeLabels (Object o)
	{
		if (o instanceof Labels)
			return (Labels)o;
		Labels labels = new Labels();
		labels.addLabel (o);
		return labels;
	}
	public static GlobalLabel	getGlobalLabel (Object o)
	{
		if (o == null)
			return null;
		if (o instanceof Labels)
			o = ((Labels)o).getGlobalLabel();
		if (o instanceof GlobalLabel)
			return (GlobalLabel)o;
		return null;
	}

	public Labels ()
	{
	}

	public Labels (Labels l) throws CloneNotSupportedException
	{
		labels = Document.cloneArrayList (l.labels);
	}

	@Override
	public	boolean equals(Object o) 
	{
		if (o == null)
			return false;
		if (o == this)
			return true;
		if (!(o instanceof Labels))
			return false;
		Labels x = (Labels)o;
		return (labels==null)?(x.labels==null):labels.equals(x.labels);
	}
	@Override 
	public	Object clone() throws CloneNotSupportedException {
		return new Labels(this);
	}

	public	void	addLabel (Object label)
	{
	    if (label == null)
			return;
	    if (labels == null)
		labels = new ArrayList<Object>();
	    else if (labels.indexOf(label) >= 0)
			return;
	    labels.add (label);
	}

	public	void	removeLabel (int i)
	{
		if (labels == null)
			return;
		if (i >= labels.size())
			return;
		labels.remove (i);
	}
	public	void	removeLabel (Object o)
	{
		if (labels == null)
			return;
		labels.remove (o);
	}

	public	boolean	isEmpty()
	{
		return getLabelCount() == 0;
	}

	public	int	getLabelCount()
	{
		if (labels == null)
			return 0;
		return labels.size();
	}

	public	Object	getLabel(int i)
        { 
	    return labels.get(i);
	}

	public	GlobalLabel	getGlobalLabel() 
	{ 
		if (labels == null)
			return null;
		for (int i=0;i<labels.size();i++)
			if (labels.get(i).getClass() == GlobalLabel.class)
				return (GlobalLabel)labels.get(i);
		return null; 
	}

	public	void		setGlobalLabel(int label,float weight)
	{
		GlobalLabel glabel = getGlobalLabel();
		if (glabel != null)
			labels.remove (glabel);
		if (weight != 0)
			addLabel (new GlobalLabel(label,weight));
	}

}

	
