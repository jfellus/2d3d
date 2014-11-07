 /**
 * \file LogResults.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.io.PrintWriter;
import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;

public class LogResults extends Document
{
	protected	TreeMap<String,LogResult> categories;

	public LogResults () {
		categories = new TreeMap<String,LogResult>();
	}
	public	void	put (String category,LogResult result) {
		categories.put (category,result);
	}
	public	LogResult get (String category) {
		return categories.get(category);
	}
	public	String[] getCategories() {
		return categories.keySet().toArray(new String[0]);
	}

        public  float  getMeanValue(String varName) {
            double mean = 0, count = 0;
            Iterator<Map.Entry<String, LogResult>> itr = categories.entrySet().iterator();
            while (itr.hasNext()) {
                Map.Entry<String, LogResult> entry = itr.next();
                LogResult log = entry.getValue();
                mean += log.get(varName).getValue(0,0);
                count++;
            }
            return (float) (mean / count);
        }

	public	void	getMeanPerFeedbackSummary (PrintWriter out,String[] varNames) {
            Iterator<Map.Entry<String, LogResult>> itr = categories.entrySet().iterator();
            float[][] all = new float[varNames.length][0];
            float count = 0;
            while (itr.hasNext()) {
                Map.Entry<String, LogResult> entry = itr.next();
                String category = entry.getKey();
                LogResult log = entry.getValue();
                out.println(category + ":");
                for (int v = 0; v < varNames.length; v++) {
                    float[] mean = log.getMeanPerFeedback(varNames[v]);
                    if (all[v].length == 0) {
                        all[v] = new float[mean.length];
                    }
                    for (int i = 0; i < mean.length; i++) {
                        if (i > 0) {
                            out.print(",");
                        }
                        out.print(mean[i]);
                        all[v][i] += mean[i];
                    }
                    out.println();
                }
                out.println();
                count++;
            }
            out.println("all:");
            for (int v = 0; v < varNames.length; v++) {
                for (int i = 0; i < all[v].length; i++) {
                    if (i > 0) {
                        out.print(",");
                    }
                    out.print(all[v][i] / count);
                }
                out.println();
            }
	}

}
