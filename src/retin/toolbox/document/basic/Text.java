/**
 * \file BytesMatrix.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */

package retin.toolbox.document.basic;

import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.util.logging.Level;
import java.util.logging.Logger;
import retin.toolbox.document.Document;

public class Text extends Document
{
    private String encoding;
    private int size;
    private byte[] chars;

    public	Text () {
        encoding = Charset.defaultCharset().name();
    }
    public      Text (String str) {
        encoding = Charset.defaultCharset().name();
        chars = str.getBytes();
        size = chars.length;
    }

    public int getSize () { return size; }
    public byte[] getChars() { return chars; }

    public  void    setCharset (String charsetName) throws UnsupportedEncodingException {
        String str = new String(chars, 0, chars.length, encoding);
        chars = str.getBytes(charsetName);
        size = chars.length;
        encoding = charsetName;
    }

    @Override
    public String toString() {
        try {
            return new String(chars, 0, chars.length, encoding);
        } catch (UnsupportedEncodingException ex) {
            return ex.getMessage();
        }
    }

}
