 /**
 * \file Parameter.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import retin.toolbox.document.Parameters;

public class Parameter implements Comparable
{
	String		value;
	Parameters	params;

	public	Parameter ()
	{
	}

	public Parameter (String value)
	{
		this.value = value;
	}
	public Parameter (String value,Parameters params)
	{
		this.value = value;
		this.params = params;
	}

	public	String 	getValue ()
	{
		return value;
	}
	public	void	setValue (String value)
	{
		this.value = value;
	}

	public Parameters getParams ()
	{
		return params;
	}
	public	void	setParams (Parameters params)
	{
		this.params = params;
	}

	public	int	compareTo (Object o)
	{
		Parameter x = (Parameter)o;

	// Value
		if (value == null)
		{
			if (x.value != null)
				return -1;
		}
		else
		{
			if (x.value == null)
				return 1;
			int c = value.compareTo(x.value);
			if (c != 0) return c;
		}

	// Parameters
		if (params == null)
		{
			if (x.params != null)
				return -1;
		}
		else
		{
			if (x.params == null)
				return 1;
			int c = params.compareTo(x.params);
			if (c != 0) return c;
		}

		return 0;
	}
	public	boolean equals(Object o)
	{
		return compareTo(o) == 0;
	}

}
