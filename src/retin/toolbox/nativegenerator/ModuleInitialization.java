/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.toolbox.nativegenerator;

import retin.toolbox.core.Library;
import retin.toolbox.database.database_operations.DatabaseOperation;

/**
 *
 * @author romanegr
 */
public class ModuleInitialization {
    static {
        Library.load("toolbox_nativegenerator");
        DatabaseOperation.register(new NativeGeneratorTables());
    }
}
