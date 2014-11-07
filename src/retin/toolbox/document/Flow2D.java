/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package retin.toolbox.document;

/**
 *
 * @author romanegr
 */
public class Flow2D {
    private int width,height;
    private float[] flowX;
    private float[] flowY;

    public	Flow2D ()
    {
            this.width = 0;
            this.height = 0;
    }
    public Flow2D (int width,int height)
    {
            this.width = width;
            this.height = height;
            this.flowX = new float[width*height];
            this.flowY = new float[width*height];
    }

    public int getWidth() { return width; }
    public int getHeight() { return height; }
    public float[] getDataX() { return flowX; }
    public float[] getDataY() { return flowY; }
    
    public void resize (int width,int height){
        if(this.width == width || this.height == height){
            this.width = width;
            this.height = height;
            this.flowX = new float[width*height];
            this.flowY = new float[width*height];
        }
    }

    public void setValueX (int i,int j,float val) 
    {
            flowX[i+j*width] = val;
    }

    public float getValueX (int i,int j) 
    {
            return flowX[i+j*width];
    }
    
    public void setValueY (int i,int j,float val) 
    {
            flowY[i+j*width] = val;
    }

    public float getValueY (int i,int j) 
    {
            return flowY[i+j*width];
    }

    public	Object 	convertTo (Class classObject) throws Exception
    {
            if (classObject.equals(getClass()))
                    return this;
            throw new Exception ("Ne sait pas convertir de "+getClass().getName()+" vers "+classObject.getName());
    }
}
