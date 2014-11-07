package retin.feature.dfeat;

import retin.toolbox.core.Library;
import retin.toolbox.database.database_operations.DatabaseOperation;
import retin.toolbox.nativegenerator.NativeDocumentGeneratorRegister;

public class ModuleInitialization {

    static {
        Library.load("feature_dfeat");
        DatabaseOperation.register(new DfeatTables());
        try {
            NativeDocumentGeneratorRegister.getInstance().registerNativeDocumentGenerator(NativeDaveFeatureGenerator.class);
        } catch (Exception ex) {
            System.out.println(ModuleInitialization.class.getName() + ":" + ex.toString());
        }
    }

}

