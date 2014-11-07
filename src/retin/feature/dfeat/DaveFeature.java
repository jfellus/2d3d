/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.feature.dfeat;


import retin.toolbox.document.ImageFile;
import retin.toolbox.document.Parameters;
import retin.toolbox.document.list.FeatureList;

public class DaveFeature {
	
		public native FeatureList extractHog(String imageFile,
											Parameters featureParams);
		public native FeatureList extractFastSift(String imageFile,
											Parameters featureParams);
		public native FeatureList extractDSift(String imageFile,
											Parameters featureParams);
		public native FeatureList extractRgb(String imageFile,
											Parameters featureParams);
	
}
