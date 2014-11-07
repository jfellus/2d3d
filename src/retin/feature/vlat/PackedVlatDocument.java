/**
 * \file PackedVlatDocument.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.feature.vlat;

import retin.toolbox.document.Document;

public class PackedVlatDocument extends Document
{
	private int     dim;
	private int     size;
	private float[] features;
        private int     bagCount;
        private int[]   bagSizes;
        private int[]   bagCounts;
}
