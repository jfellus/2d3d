/**
 * \file StringList.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.list;

import java.util.ArrayList;
import retin.toolbox.document.Document;

public class StringList extends Document
{
	private ArrayList<String> strings;

	public	StringList ()
	{
		strings = new ArrayList<String>();
	}
        
        public	StringList (ArrayList<String> strings)
	{
		this.strings = strings;
	}

	public	int	size() { return strings.size(); }
	public	String	get(int i) { return strings.get(i); }
	public	void	add(String s) { strings.add(s); }
}
