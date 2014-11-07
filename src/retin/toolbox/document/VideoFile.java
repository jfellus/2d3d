 /**
 * \file VideoFile.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document;

import retin.toolbox.codec.VideoDecoder;

public class VideoFile extends FileName
{
	int	width,height;
	double	fps;
	long	start,duration;

	public VideoFile ()
	{
		super ();
		width = 0;
		height = 0;
		fps = 25;
		start = 0;
		duration = 0;
	}
	public VideoFile (String file)
	{
		super (file);
		width = 0;
		height = 0;
		fps = 25;
		start = 0;
		duration = 0;
	}
        public  VideoFile (VideoFile video)
        {
            super(video.file);
            width = video.width;
            height = video.height;
            fps = video.fps;
            start = video.start;
            duration = video.duration;
        }
	
	public int	getWidth () { return width; }
	public int	getHeight () { return height; }
	public long	getStart () { return start; }
	public long	getDuration () { return duration; }
	
	public void	setWidth (int x) { width = x; }
	public void	setHeight (int x) { height = x; }
	public void	setStart (long x) { start = x; }
	public void	setDuration (long x) { duration = x; }

	public VideoDecoder loadHeader (String basePath) throws Exception
	{
		VideoDecoder decoder = new VideoDecoder ();
		decoder.open (getAbsoluteFile(basePath).toString());
		width = decoder.getFrameWidth();
		height = decoder.getFrameHeight();
		fps = decoder.getFrameCountPerSecond();
		start = 0;
		duration = decoder.getFrameCount();
		return decoder;
	}
	
}
