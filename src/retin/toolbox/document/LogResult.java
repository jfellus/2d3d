 /**
 * \file LogResult.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;

public class LogResult extends Document
{
	protected	TreeMap<String,FloatMatrix> vars;

	public LogResult () {
		vars = new TreeMap<String,FloatMatrix>();
	}
	public LogResult (String[] varNames,int sessionCount,int feedbackSteps ) {
		vars = new TreeMap<String,FloatMatrix>();
		for (int i=0;i<varNames.length;i++)
			vars.put(varNames[i],new FloatMatrix(sessionCount,feedbackSteps));
	}
	public LogResult (String[] varNames) {
		vars = new TreeMap<String,FloatMatrix>();
		for (int i=0;i<varNames.length;i++)
			vars.put(varNames[i],new FloatMatrix(1,1));
	}
	public	void	put (String var,FloatMatrix result) {
		vars.put (var,result);
	}
	public	FloatMatrix get (String var) {
		return vars.get(var);
	}
	public	String[] getVarNames() {
		return vars.keySet().toArray(new String[0]);
	}
	public	void	resize (int sessionCount,int feedbackSteps) {
		Iterator<Map.Entry<String,FloatMatrix>> itr = vars.entrySet().iterator();
		while(itr.hasNext())
			itr.next().getValue().resize(sessionCount,feedbackSteps);
	}
	public	void	setValue (String varName,int session,int feedback,float value) {
		FloatMatrix m = vars.get(varName);
		m.setMinSize(session+1,feedback+1);
		m.setValue(session,feedback,value);
	}

	public	float[]	getMeanPerFeedback (String varName) {
		return vars.get(varName).meanRow();
	}
}
