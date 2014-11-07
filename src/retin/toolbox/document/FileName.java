 /**
 * \file FileName.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.io.File;

public class FileName extends Document
{
	protected String	file; 

	public FileName ()
	{
	}

	public FileName (String file)
	{
		this.file = file;
	}

        public void     setName(String file) {
            this.file = file;
        }

	public String getName() {
		return file;
	}

	public File getAbsoluteFile(String basePath)
	{
		File f = new File(file);
		if (!basePath.isEmpty() && !f.isAbsolute())
			return new File(basePath,file);
		return f;
	}
        
        public boolean exists(String dir){
            File f = new File(dir + "/" + file);
            if(f.exists() && !f.isDirectory()){
                return true;
            }
            return false;
        }
        
}

