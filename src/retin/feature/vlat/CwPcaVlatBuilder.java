
package retin.feature.vlat;

import java.util.List;
import retin.toolbox.database.Table;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.list.FeatureList;


public class CwPcaVlatBuilder {

    protected FeatureList centers;
    protected List dirs, means, vars;

    public CwPcaVlatBuilder(FeatureList centers) {
        this.centers = centers;
    }

    public CwPcaVlatBuilder(FeatureList centers, List dirs, List means, List vars) {
        this.centers = centers;
        this.dirs = dirs;
        this.means = means;
        this.vars = vars;
    }

    public native void       compDict (Table bagTable, String[] keys, int maxDim, double maxEnergy);
    public native Feature    compMeanVlat (Table bagTable, String[] keys,int order,String format);
    public native Feature    compVlat (FeatureList input,int order,String format);

    public List getDirs() {
        return dirs;
    }

    public List getMeans() {
        return means;
    }

    public List getVars() {
        return vars;
    }
}
