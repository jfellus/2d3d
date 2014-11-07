/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.toolbox.nativegenerator;

import java.util.AbstractMap;
import java.util.HashSet;
import java.util.Set;
import retin.toolbox.database.Manager;
import retin.toolbox.database.Table;
import retin.toolbox.document.Document;
import retin.toolbox.document.Options;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.basic.NativePointer;

/**
 *
 * @author romanegr
 */
public abstract class NativeDocumentGenerator {
    protected NativePointer instance;
    
    protected Set<String> tags;
    
    public static String IMAGETYPE = "image";
    public static String VIDEOTYPE = "video";
    public static String FILE3DTYPE = "file3d";
    
    public NativeDocumentGenerator(){
        instance = null;
        tags = new HashSet<String>();
    }
    
    public NativePointer getInstance(){
        return instance;
    }
    
    protected void addTag(String tag) {
        tags.add(tag);
    }
    
    public boolean containsTag(String [] listtags) {
        boolean res = true;
        for (String x : listtags){    
            res = res && tags.contains(x);
        }
        return res;
    }
    
    public void initParentTable(Manager manager, String baseName, String tableName){
        initParentTable(manager.getTable(baseName, tableName));
    }
    
    public abstract String getName();
    
    public abstract String[] getSupportedInputType();
    
    public abstract Options getOptions(Manager manager, AbstractMap<String, String> args);
    
    public abstract String getDescription(Parameters params);
    
    public native int getDim(Parameters params);
    
    public native void initParentTable(Table parentTable);
    
    public native void initParams(Parameters params) throws Exception;
    
    public native void reset();
    
    public native Document computeDocument(String key) throws Exception;
    
    public native boolean isInitialized();
    
    /*private native void newIntance();
    private native void deleteIntance();
    
    @Override
    protected void finalize() throws Throwable{
        deleteIntance();
        super.finalize();
    }*/
}
