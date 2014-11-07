/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.toolbox.nativegenerator;

import retin.toolbox.document.list.FeatureList;

/**
 *
 * @author romanegr
 */
public abstract class NativeFeatureListGenerator extends NativeDocumentGenerator {

    public FeatureList computeFeatureList(String key) throws Exception {
        return (FeatureList) computeDocument(key);
    }
    
}
