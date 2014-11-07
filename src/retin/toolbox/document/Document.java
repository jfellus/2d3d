 /**
 * \file Document.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.awt.image.BufferedImage;
import java.util.*;
 
public abstract class Document implements Cloneable
{
	@SuppressWarnings("unchecked")
	public	static	Object	cloneDocument (Object input) throws CloneNotSupportedException {
		if (input == null)
			return null;
		if (input instanceof Document)
			return ((Document)input).clone();
		else if (input instanceof ArrayList)
			return Document.cloneArrayList((AbstractList)input);
		else
			throw new CloneNotSupportedException(input.getClass().getName()+" clone() unsupported");
	}
	public	static	ArrayList<Object> cloneArrayList (AbstractList input) throws CloneNotSupportedException {
		if (input == null)
			return null;
		ArrayList<Object> output = new ArrayList<Object>();
		for (int i=0;i<input.size();i++) {
			output.add( Document.cloneDocument(input.get(i)) );
		}
		return output;
	}
	public	static	TreeMap<String,Object> cloneTreeMap (AbstractMap<String,Object> input) throws CloneNotSupportedException {
		if (input == null)
			return null;
		TreeMap<String,Object> output = new TreeMap<String,Object>();
		Iterator< Map.Entry<String,Object> > it = input.entrySet().iterator();
		while (it.hasNext()) 
		{
			Map.Entry<String,Object> entry = it.next();
			output.put( entry.getKey(),Document.cloneDocument(entry.getValue()) );
		}
		return output;
	}



	public	Object 	convertTo (Class classObject) throws Exception
	{
		if (classObject.equals(getClass()))
			return this;
		throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
	}

	public	void show ()
	{
		System.out.println (toXMLString());
	}

	@Override
	public Object		clone() throws CloneNotSupportedException { throw new CloneNotSupportedException(getClass().getName()+" clone() unsupported"); }
	@Override
	public String 		toString () { return toXMLString(); }
	public BufferedImage	toBufferedImage () throws Exception { throw new Exception("Conversion to BufferedImage unsupported"); }

	public native void   	fromXMLString ();
	public native String 	toXMLString ();
	public static native String	makeXMLString (Object o);
}
