 /**
 * \file ImageFile.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import java.io.File;
import java.awt.image.BufferedImage;

public class ImageFile extends FileName
{
	int	width,height;

	public ImageFile ()
	{
		super ();
		width = 0;
		height = 0;
	}
	public ImageFile (String file)
	{
		super (file);
		width = 0;
		height = 0;
	}

	public void setFile (String file)
	{
		this.file = file;
		width = 0;
		height = 0;
	}

	public	void	setWidth (int x) { width = x; }
	public	void	setHeight (int x) { height = x; }

	public	int	getWidth() { return width; }
	public	int	getHeight() { return height; }

	public BufferedImage load (String basePath) throws java.io.IOException
	{
		BufferedImage image;
		image = javax.imageio.ImageIO.read(getAbsoluteFile(basePath)); 
		width = image.getWidth();
		height = image.getHeight();
		return image;
	}

	public void save (BufferedImage image,String basePath) throws java.io.IOException
	{
		width = image.getWidth();
		height = image.getHeight();
		File file = getAbsoluteFile(basePath);
		File parent = file.getParentFile();
		if (parent != null)
			parent.mkdirs();
		javax.imageio.ImageIO.write(image,"png",file); 
	}
}
