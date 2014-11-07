 /**
 * \file Core.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.serialization;

import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Java
{
	public static native float[] 	loadFloatBuffer (String file,int offset,int floatCount);
	public static native void 	saveFloatBuffer (String file,float[] buffer);

	public static native String 	shellExec (String cmd);
        
        public static long              systemMemory() {
            String str = shellExec("cat /proc/meminfo | grep MemTotal");
            if (str == null || str.isEmpty())
                return 0;
            Pattern pattern = Pattern.compile("[^0-9]+([0-9]+)[^0-9]+");
            Matcher matcher = pattern.matcher(str);
            if (matcher.matches() && matcher.groupCount() == 1) {
                return Long.parseLong(matcher.group(1))*1024;
            }
            return 0;
        }

	public static int[] sort (float[] array) {
		int[] index = new int[array.length];
		for (int i=0;i<index.length;i++)
			index[i] = i;
		sort(array,index,0,array.length-1);
		return index;
	}
	private static void sort (float[] a,int[] index,int left,int right) {
		if (left >= right)
			return;
		float p = a[index[left]];
		int i = left+1;
		int j = right;
		while (true) {
			while (i < j && a[index[i]] <= p) i++;
			while (i < j && a[index[j]] > p) j--;
			if (i >= j) break;
			int tmp = index[i];
			index[i] = index[j];
			index[j] = tmp;
		}
		if (a[index[i]] > p) i --;
		int tmp = index[i];
		index[i] = index[left];
		index[left] = tmp;
		sort(a,index,left,i-1);
		sort(a,index,i+1,right);
	}

}

