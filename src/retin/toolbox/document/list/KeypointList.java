/**
 * \file KeypointList.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */
package retin.toolbox.document.list;

import java.util.AbstractList;
import java.util.ArrayList;
import java.awt.*;
import java.awt.image.BufferedImage;
import retin.toolbox.document.Document;
import retin.toolbox.document.basic.Feature;
import retin.toolbox.document.basic.Keypoint;

public class KeypointList extends Document {

    private int count;
    private AbstractList<Keypoint> keypoints;

    public KeypointList() {
        keypoints = new ArrayList<Keypoint>();
    }

    public int getCount() {
        return count;
    }

    public void add(Keypoint kp) {
        keypoints.add(kp);
        count = keypoints.size();
    }

    public Keypoint get(int i) {
        return keypoints.get(i);
    }

    public Feature getScores() {
        Feature f = new Feature(count);
        for (int i = 0; i < count; i++) {
            f.setValue(i, keypoints.get(i).getScore());
        }
        return f;
    }

    public KeypointList subList(int fromIndex, int toIndex) {
        KeypointList list = new KeypointList();
        list.keypoints = (AbstractList<Keypoint>) keypoints.subList(fromIndex, toIndex);
        list.count = list.keypoints.size();
        return list;
    }

    public void draw(BufferedImage image, float scale) {
        Graphics2D g = image.createGraphics();
        for (int i = 0; i < count; i++) {
            keypoints.get(i).draw(g, scale, scale);
        }
        g.dispose();
    }

    public Rectangle getBoundingBox() {
        Rectangle r = new Rectangle();
        for (int i = 0; i < keypoints.size(); i++) {
            Keypoint kp = keypoints.get(i);
            r.add((int) kp.getX(), (int) kp.getY());
        }
        return r;
    }

    /*	public	void	show ()
    {
    System.out.println(count+" keypoints");
    for (int i=0;i<count;i++)
    keypoints.get(i).show();
    }*/
}
