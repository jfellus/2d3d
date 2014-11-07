/**
 * \file BytesList.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */
package retin.toolbox.document.list;

import java.awt.image.*;
import java.util.Arrays;
import retin.toolbox.document.*;

public class BytesList extends Document {

    private int dim;
    private int size;
    private byte[] bytes;

    public BytesList() {
        this.size = 0;
        this.dim = 0;
    }

    public BytesList(int dim, int size) {
        this.size = size;
        this.dim = dim;
        this.bytes = new byte[dim * size];
    }

    public BytesList(BytesMatrix map) {
        this.dim = map.getDim();
        this.size = map.getWidth() * map.getHeight();
        this.bytes = new byte[dim * size];
        System.arraycopy(map.getBytes(), 0, bytes, 0, dim * size);
    }

    public BytesList(FeatureList list) {
        dim = list.getDim();
        size = list.getSize();
        bytes = new byte[dim * size];
        float[] features = list.getFeatures();
        for (int i = 0; i < bytes.length; i++) {
            int x = (int) (features[i]);
            if (x < 0) {
                x = 0;
            } else if (x > 255) {
                x = 255;
            }
            bytes[i] = (byte) (x);
        }
    }

    public BytesList(BufferedImage image) throws Exception {
        size = image.getWidth() * image.getHeight();

        if (image.getType() == BufferedImage.TYPE_3BYTE_BGR) {
            dim = 3;
            bytes = new byte[size * dim];
            byte[] buffer = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
            for (int i = 0; i < size; i++) {
                bytes[3 * i + 0] = buffer[3 * i + 2];
                bytes[3 * i + 1] = buffer[3 * i + 1];
                bytes[3 * i + 2] = buffer[3 * i + 0];
            }
        } else {
            throw new Exception("BytesList(BufferedImage) Unsupported format");
        }
    }

    public int getDim() {
        return dim;
    }

    public int getSize() {
        return size;
    }

    public void setBytes(byte[] bytes) {
        this.bytes = bytes;
    }

    public byte[] getBytes() {
        return bytes;
    }

    public void setValue(int k, int i, int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 255) {
            x = 255;
        }
        bytes[k + i * dim] = (byte) x;
    }

    public BytesList subList(int fromIndex, int toIndex) {
        if (fromIndex < 0)
            throw new IllegalArgumentException("Invalid fromIndex");
        if (toIndex > size)
            throw new IllegalArgumentException("Invalid toIndex");
        int len = toIndex-fromIndex;
        if (len < 0) len = 0;
        BytesList list = new BytesList(dim,toIndex-fromIndex);
        System.arraycopy(bytes, fromIndex * dim, list.getBytes(), 0, len * dim);
        return list;
    }

    @Override
    public Object convertTo(Class classObject) throws Exception {
        if (classObject.equals(getClass())) {
            return this;
        }
        if (classObject.equals(FeatureList.class)) {
            return new FeatureList(this);
        }
        throw new Exception("Ne sait pas convertir de " + getClass().getName() + " vers " + classObject.getName());
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        }
        if (!(o instanceof BytesList)) {
            return false;
        }
        BytesList x = (BytesList) o;
        if (x.dim != dim || x.size != size) {
            return false;
        }
        return Arrays.equals(x.bytes, bytes);
    }

    @Override
    public BufferedImage toBufferedImage() throws Exception {
        BufferedImage image = null;
        byte[] buffer = null;
        if (dim == 1) {
            image = new BufferedImage(size, 1, BufferedImage.TYPE_BYTE_GRAY);
            buffer = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
            System.arraycopy(bytes, 0, buffer, 0, size);
        } else if (dim == 3) {
            image = new BufferedImage(size, dim, BufferedImage.TYPE_3BYTE_BGR);
            buffer = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
            for (int i = 0; i < size; i++) {
                buffer[3 * i + 0] = bytes[3 * i + 2];
                buffer[3 * i + 1] = bytes[3 * i + 1];
                buffer[3 * i + 2] = bytes[3 * i + 0];
            }
        } else {
            throw new Exception("convertToBufferedImage() Invalid dimensions");
        }
        return image;
    }
}
