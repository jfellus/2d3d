/*
Copyright © CNRS 2012. 
Authors: Philippe-Henri Gosselin, David Picard, Romain Négrel, Olivier Kihl
Contact: philippe-henri.gosselin@ensea.fr

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

 */
/**
 * \file NativeGenerateLowLevelVideo.java
 * \author David Picard
 * \version 4.2
 */
 
 package retin.feature.depthimages;
 
import java.util.AbstractMap;
import java.util.Map;
import java.util.Arrays;
import retin.toolbox.database.Manager;
import retin.toolbox.document.Options;
import retin.toolbox.document.Parameters;
import retin.toolbox.nativegenerator.NativeDocumentGenerator;
import retin.toolbox.nativegenerator.NativeFeatureListGenerator;
 
 public class DepthImagesGenerator extends NativeFeatureListGenerator {
 
	public final static String NB_PROJECTIONS = "nb_projections";
	public final static String IMAGE_WIDTH = "image_width";
	public final static String IMAGE_HEIGHT = "image_height";
	public final static String SPACING = "spacing";
	public final static String SCALES = "scales";
        
        public static String NAME = "depthimages";

    public DepthImagesGenerator(){
        newIntance();
    }
        
    @Override
    protected void finalize() throws Throwable{
        deleteIntance();
        super.finalize();
    }
    
    public String getName() {
        return NAME;
    }

    @Override
    public String[] getSupportedInputType() {
        String list[] = {NativeDocumentGenerator.FILE3DTYPE};
        return list;
    }

    @Override
    public Options getOptions(Manager manager, AbstractMap<String, String> args) {
        Options options = new Options();
		options.addInteger(NB_PROJECTIONS).setRange(0, 10000).setDefault(0);
		options.addInteger(IMAGE_HEIGHT).setRange(1, 4096).setDefault(256);
		options.addInteger(IMAGE_WIDTH).setRange(1, 4096).setDefault(256);
        options.addInteger(SPACING).setRange(1, 100).setDefault(4);
        options.addStringValues(SCALES).addDefault("10").addDefault("14").addDefault("18").addDefault("22");
        return options;
    }
    
    public String getDescription(Parameters params) {
		int proj = params.getAsInt(NB_PROJECTIONS);
		int widt = params.getAsInt(IMAGE_WIDTH);
		int heig = params.getAsInt(IMAGE_HEIGHT);
		String scales[] = params.getParams(SCALES).getNames();
		String tos = Arrays.toString(scales).replaceAll("\\[", "").replaceAll("\\]", "").replaceAll(",", "+").replaceAll(" ", "");
		int spacing = params.getAsInt(SPACING);
		
		return widt+"x"+heig+"@"+proj+"_"+tos+"@"+spacing;
    }
    
    private native void newIntance();
    private native void deleteIntance();
 
 }
 
