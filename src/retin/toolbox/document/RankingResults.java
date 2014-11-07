 /**
 * \file RankingResults.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.util.TreeMap;

public class RankingResults extends Document
{

	protected	TreeMap<String,FloatMatrix> results;

	public RankingResults () {
		results = new TreeMap<String,FloatMatrix>();
	}

	public boolean has(String category) {
		return results.get(category) != null;
	}
	public	void	put (String category,FloatMatrix result) {
		results.put (category,result);
	}
	public	FloatMatrix get (String category) {
		return results.get(category);
	}
	public	String[] getCategories() {
		return results.keySet().toArray(new String[0]);
	}

}
