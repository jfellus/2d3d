package retin.feature.vlat;

import retin.toolbox.core.Library;
import retin.toolbox.database.database_operations.DatabaseOperation;

public class ModuleInitialization {

    static {
       
        DatabaseOperation.register(new VlatTables());

        Library.load("feature_vlat");
        init();
    }

    public static native void init();

}
