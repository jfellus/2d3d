 /**
 * \file Parameters.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;


import java.util.Map;
import java.util.TreeMap;
import java.util.Iterator;

public class Parameters extends Document implements Comparable
{
        public static Parameters makeParameters(String str) {
            try {
                return new Parameters(str);
            }
            catch (Exception ex) {
                return null;
            }
        }

        TreeMap<String,Parameter>	values;

	public	Parameters ()
	{
		values = new TreeMap<String,Parameter>();
	}
	public	Parameters (String str)
	{
		values = new TreeMap<String,Parameter>();
		set (str);
	}
	public Parameters copy()
	{
		try {
			return new Parameters(toString());
		}
		catch (Exception ex) {
		}
		return null;
	}

	protected int i;

	public boolean isAlphaNum (char c)
	{
		return c > 32 && (c != '=') && (c != '{') && (c != '}') && (c != ',');
	}
	public boolean isAlphaNum (String str)
	{
		if (str == null)
			return false;
		for (int i=0;i<str.length();i++)
			if (!isAlphaNum(str.charAt(i)))
				return false;
		return true;
	}

	public String nextWord (String str)
	{
		// Saute les espaces
		while (i<str.length() && str.charAt(i) <= 32) i++;
		if (i >= str.length())
			return null;

		char c = str.charAt(i);
		int i0 = i++;
		if (c == '"')
		{
			i0 ++;
			while (i<str.length() && str.charAt(i) != '"') i++;
			if (i >= str.length())
				throw new IllegalArgumentException("Symbole '\"' attendu");
			return str.substring(i0,i++);
		}
		else if (c == '\'')
		{
			i0 ++;
			while (i<str.length() && str.charAt(i) != '\'') i++;
			if (i >= str.length())
				throw new IllegalArgumentException("Symbole '\"' attendu");
			return str.substring(i0,i++);
		}
		else if (c == '{')
		{
			i0 ++;
			int level = 0;
			while (true)
			{
				String word = nextWord(str);
				if (word == null)
					throw new IllegalArgumentException("Symbole '}' attendu");
				else if (word.equals("}"))
				{
					if (level == 0)
						break;
					level --;
				}
				else if (word.equals("{"))
					level ++;
			}
			return str.substring(i0,i-1);
		}
		else if (isAlphaNum(c))
		{
			while (i<str.length())
			{
				c = str.charAt(i);
				if (!isAlphaNum(c))
					break;
				i++;
			}
		}
		return str.substring(i0,i);
	}

	public	void	set(String str)
	{
		if (str == null)
			return;
		i = 0;
		while (true)
		{
			String name = nextWord(str);
			if (name == null)
				break;
			if (name.equals(","))
				continue;
			String word = nextWord(str);
			if (word == null || word.equals(","))
			{
				set (name,null);
			}
			else if (word.equals("="))
			{
				String value = nextWord(str);
				if (value == null)
					throw new IllegalArgumentException("Valeur attendue après '"+name+"='");
				word = nextWord(str);
				if (word == null || word.equals(","))
					set (name,value);
				else
					set (name,value,word);
			}
			else
			{
				set (name,null,word);
			}
		}
	}

	public	Parameters	set (String name,String value)
	{
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException ("Nom vide");
		if (value == null || value.isEmpty())
			values.put (name,new Parameter(null));
		else
			values.put (name,new Parameter(value));
                return this;
	}
        public  Parameters      set(String name,int x) {
            set(name,String.valueOf(x));
            return this;
        }
        public  Parameters      set(String name,double x) {
            set(name,String.valueOf(x));
            return this;
        }
	public	void	set (String name,String value,String params)
	{
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException ("Nom vide");
		if (params == null)
		{
			if (value == null || value.isEmpty())
				values.put (name,new Parameter(null));
			else
				values.put (name,new Parameter(value));
		}
		else
		{
			values.put (name,new Parameter(value,new Parameters(params)));
		}
	}
	public	void	set (String name,String value,Parameters params)
	{
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException ("Nom vide");
		values.put (name,new Parameter(value,params));
	}
        public Parameters set (Parameters p) {
            Iterator< Map.Entry<String,Parameter> > it = p.values.entrySet().iterator();
            while (it.hasNext())
            {
                    Map.Entry<String,Parameter> entry = it.next();
                    setParameter(entry.getKey(),entry.getValue());
            }
            return this;
        }

	public	void	remove (String key)
	{
		values.remove (key);
	}
	public	void	clear ()
	{
			values = new TreeMap<String,Parameter>();
	}
	public	int		getNameCount()
	{
		return values.size();
	}
	public	String[]	getNames ()
	{
		return values.keySet().toArray(new String[0]);		
	}
	public	Parameter	getParameter (String key)
	{
		return values.get(key);
	}
	public	void		setParameter (String key,Parameter p)
	{
		values.put(key,p);
	}


	public  boolean has (String key)
	{
		return values.get(key) != null;
	}

	public	String	get (String key)
	{
		Parameter param = values.get(key);
		if (param == null)
			throw new IllegalArgumentException("No parameter '"+key+"'");
		String value = param.getValue();
		if (value == null)
			return "";
		return value;
	}
	public String get(String key, String defaut) {
		Parameter param = values.get(key);
		if (param == null)
			return defaut;
		String value = param.getValue();
		if (value == null)
			return defaut;
		return value;
	}

	public	int	getAsInt (String key)
	{
		return Integer.parseInt (get(key));
	}
	public	int	getAsInt (String key,int defaut)
	{
		try {
			return Integer.parseInt (get(key));
		}
		catch (Exception e) {
			return defaut;
		}
	}
                
	public	double	getAsDouble (String key) throws Exception
	{
		return Double.parseDouble (get(key));
	}
	public	double	getAsDouble (String key, double defaut)
	{
		try {
			return Double.parseDouble (get(key));
		}
		catch (Exception e) {
			return defaut;
		}
	}

	public  boolean hasParams (String key)
	{
		Parameter param = values.get(key);
		if (param == null)
			return false;
		return param.getParams() != null;
	}
	public	Parameters	getParams (String key)
	{
		Parameter param = values.get(key);
		if (param == null)
			throw new IllegalArgumentException("No parameter "+key);
		Parameters params = param.getParams();
		if (params == null)
			throw new IllegalArgumentException("Parameter "+key+" has no sub parameters");
		return params;
	}

	public	String	toString (String key)
	{
		String result;
		if (isAlphaNum(key))
			result = key;
		else
			result = "'"+key+"'";
		Parameter param = values.get(key);
		if (param != null)
		{
			String value = param.getValue();
			if (value != null && !value.isEmpty())
			{
				if (isAlphaNum(value))
					result += "="+value;
				else
					result += "='"+value+"'";
			}
			Parameters params = param.getParams();
			if (params != null)
				result += "{"+params.toString()+"}";
		}
		return result;
	}

	public	String	toString ()
	{
		String result = "";
		Iterator< Map.Entry<String,Parameter> > it = values.entrySet().iterator();
		while (it.hasNext()) 
		{
			if (!result.isEmpty())
				result += ",";
			Map.Entry<String,Parameter> entry = it.next();
			result += toString(entry.getKey());
		}
		return result;
	}


	public	String	toString (String key,int shift)
	{
		String result = "";
                for (int s=0;s<shift;s++)
                    result += "    ";
		if (isAlphaNum(key))
			result += key;
		else
			result += "'"+key+"'";
		Parameter param = values.get(key);
		if (param != null)
		{
			String value = param.getValue();
			if (value != null && !value.isEmpty())
			{
				if (isAlphaNum(value))
					result += "="+value;
				else
					result += "='"+value+"'";
			}
			Parameters params = param.getParams();
			if (params != null) {
                                String tmp = params.toString(shift+1);
                                if (tmp.contains("\n")) {
                                    result += "{\n"+tmp+"\n";
                                    for (int s=0;s<shift;s++)
                                        result += "    ";
                                    result += "}";
                                }
                                else {
                                    result += "{"+params.toString(0)+"}";
                                }
                        }
		}
		return result;
	}

  	public	String	toString (int shift)
	{
		String result = "";
		Iterator< Map.Entry<String,Parameter> > it = values.entrySet().iterator();
		while (it.hasNext())
		{
			if (!result.isEmpty())
				result += ",\n";
			Map.Entry<String,Parameter> entry = it.next();
			result += toString(entry.getKey(),shift);
		}
		return result;
	}

	@Override
	public	boolean equals(Object o)
	{
		return compareTo(o) == 0;
	}
	@Override
	public	int	compareTo (Object o)
	{
		if (this == o)
			return 0;

		Parameters x = (Parameters)o;

		int c = values.size()-x.values.size();
		if (c != 0) return c;

		Iterator< Map.Entry<String,Parameter> > ita = values.entrySet().iterator();
		Iterator< Map.Entry<String,Parameter> > itb = x.values.entrySet().iterator();
		while (ita.hasNext()) 
		{
			Map.Entry<String,Parameter> entrya = ita.next();
			Map.Entry<String,Parameter> entryb = itb.next();
			c = entrya.getKey().compareTo(entryb.getKey());
			if (c != 0) return c;
			c = entrya.getValue().compareTo(entryb.getValue());
			if (c != 0) return c;
		}
		return 0;
	}
}
