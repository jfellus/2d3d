 /**
 * \file MeshFile.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

public class MeshFile extends FileName
{
	int	nodeCount,edgeCount;

	public MeshFile ()
	{
		super ();
	}
	public MeshFile (String file)
	{
		super (file);
	}

	public void setFile (String file)
	{
		this.file = file;
	}

}
