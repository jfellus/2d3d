package retin.toolbox.document.serialization;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.Writer;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.util.AbstractSet;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class JsonWriter {

    Writer out;
    int[] stack;
    int mode, level;
    protected static final int START_OBJECT = 100;
    protected static final int IN_OBJECT = 101;
    protected static final int START_KEY = 210;
    protected static final int START_ARRAY = 300;
    protected static final int IN_ARRAY = 301;

    public JsonWriter(Writer out) throws IOException {
        this.out = out;
        stack = new int[100];
    }

    public JsonWriter object() throws IOException {
        switch (mode) {
            case IN_ARRAY:
                out.append(',');
            case 0:
            case START_ARRAY:
                stack[level++] = IN_ARRAY;
                mode = START_OBJECT;
                out.append('{');
                return this;
            case START_KEY:
                stack[level++] = IN_OBJECT;
                mode = START_OBJECT;
                out.append(":{");
                return this;
        }
        throw new IllegalArgumentException("object()");
    }

    public JsonWriter endObject() throws IOException {
        if (level <= 0) {
            throw new IllegalArgumentException();
        }
        if (mode == START_OBJECT || mode == IN_OBJECT) {
            level--;
            mode = stack[level];
            out.append('}');
            return this;
        }
        throw new IllegalArgumentException("endObject()");
    }

    public JsonWriter array() throws IOException {
        switch (mode) {
            case IN_ARRAY:
                out.append(',');
            case 0:
            case START_ARRAY:
                stack[level++] = mode;
                mode = START_ARRAY;
                out.append('[');
                return this;
            case START_KEY:
                stack[level++] = IN_OBJECT;
                mode = START_ARRAY;
                out.append(":[");
                return this;
        }
        throw new IllegalArgumentException("array()");
    }

    public JsonWriter endArray() throws IOException {
        if (level <= 0) {
            throw new IllegalArgumentException();
        }
        if (mode == START_ARRAY || mode == IN_ARRAY) {
            level--;
            mode = stack[level];
            out.append(']');
            return this;
        }
        throw new IllegalArgumentException("endArray()");
    }

    protected JsonWriter addValue(String value) throws IOException {
        switch (mode) {
            case START_KEY:
                mode = IN_OBJECT;
                out.append(':');
                break;
            case START_ARRAY:
                mode = IN_ARRAY;
                break;
            case IN_ARRAY:
                out.append(',');
                break;
            default:
                throw new IllegalArgumentException("addValue");
        }
        out.append(value);
        return this;
    }

    public JsonWriter value(Object object) throws IOException {
        if (object == null) {
            return value();
        }
        if (object instanceof Jsonable) {
            ((Jsonable) object).toJsonValue(this);
            return this;
        }
        if (object.getClass().isArray()) {
            return value((Object[]) object);
        }
        if (object instanceof List) {
            return value((List) object);
        }
        if (object instanceof Map) {
            return value((Map) object);
        }
        if (object instanceof Set) {
            return value((Set) object);
        }
        if (object instanceof String) {
            return value((String) object);
        }
        if (object instanceof Integer) {
            return value((Integer) object);
        }
        if (object instanceof Long) {
            return valueLong((Long) object);
        }
        if (object instanceof Double) {
            return value((Double) object);
        }
        if (object instanceof Boolean) {
            return value((Boolean) object);
        }
        throw new IllegalArgumentException("Unsuppored class " + object.getClass().getName());
    }

    public JsonWriter value() throws IOException {
        return addValue("null");
    }

    public JsonWriter value(String value) throws IOException {
        return addValue(quote(value));
    }

    public JsonWriter value(int value) throws IOException {
        return addValue(String.valueOf(value));
    }

    public JsonWriter valueLong(long value) throws IOException {
        return addValue(String.valueOf(value));
    }

    public JsonWriter value(double value) throws IOException {
        return addValue(String.valueOf(value));
    }

    public JsonWriter value(boolean value) throws IOException {
        return addValue(value ? "true" : "false");
    }

    public JsonWriter value(Object[] array) throws IOException {
        array();
        for (int i = 0; i < array.length; i++) {
            value(array[i]);
        }
        endArray();
        return this;
    }

    public JsonWriter value(Object[] array, int i1, int count) throws IOException {
        if (i1 < 0) {
            i1 = 0;
        }
        if (count < 0) {
            count = array.length;
        }
        if (i1 + count > array.length) {
            count = array.length - i1;
        }
        array();
        for (int i = 0; i < count; i++) {
            value(array[i1 + i]);
        }
        endArray();
        return this;
    }

    public JsonWriter value(List list) throws IOException {
        array();
        for (int i = 0; i < list.size(); i++) {
            value(list.get(i));
        }
        endArray();
        return this;
    }

    public JsonWriter value(Map map) throws IOException {
        array();
        if (map != null) {
            Iterator ite = map.entrySet().iterator();
            while (ite.hasNext()) {
                Map.Entry entry = (Map.Entry) ite.next();
                String id = (String) entry.getKey();
                Object value = entry.getValue();
                if (id != null || value != null) {
                    object();
                    if (id != null) {
                        key("key").value(id);
                    }
                    if (value != null) {
                        key("value").value(value);
                    }
                    endObject();
                }
            }
        }
        endArray();
        return this;
    }

    public JsonWriter value(Set set) throws IOException {
        array();
        Iterator ite = set.iterator();
        while (ite.hasNext()) {
            String id = (String) ite.next();
            if (id != null) {
                value(id);
            }
        }
        endArray();
        return this;
    }

    public JsonWriter fields(Object x) throws IOException, IllegalAccessException {
        Class jclass = x.getClass();

        Field[] fields = jclass.getFields();
        for (int i=0;i<fields.length;i++) {
            Field f = fields[i];
            key(f.getName()).value(f.get(x));
        }

        return this;
    }

    public static void generateGwtClass(PrintWriter out,Class jclass) {
        Field[] fields = jclass.getFields();
        for (int i=0;i<fields.length;i++) {
            Field f = fields[i];
            String name = f.getName();
            String type = f.getType().getSimpleName();
            if (type.equals("long"))
                type = "int";
            out.println("\tpublic final native "+type+" get"+name.substring(0,1).toUpperCase()+name.substring(1)+"() /*-{");
            out.println("\t\treturn this."+name+";");
            out.println("\t}-*/;");
            out.println();
        }
    }

    public JsonWriter key(String value) throws IOException {
        switch (mode) {
            case START_OBJECT:
                mode = START_KEY;
                break;
            case IN_OBJECT:
                mode = START_KEY;
                out.append(',');
                break;
            default:
                throw new IllegalArgumentException("key()");
        }
        out.append(value);
        return this;
    }

    public static String quote(String string) {
        if (string == null || string.length() == 0) {
            return "\"\"";
        }

        char b;
        char c = 0;
        String hhhh;
        int i;
        int len = string.length();
        StringBuilder sb = new StringBuilder(len + 4);

        sb.append('"');
        for (i = 0; i < len; i += 1) {
            b = c;
            c = string.charAt(i);
            switch (c) {
                case '\\':
                case '"':
                    sb.append('\\');
                    sb.append(c);
                    break;
                case '/':
                    if (b == '<') {
                        sb.append('\\');
                    }
                    sb.append(c);
                    break;
                case '\b':
                    sb.append("\\b");
                    break;
                case '\t':
                    sb.append("\\t");
                    break;
                case '\n':
                    sb.append("\\n");
                    break;
                case '\f':
                    sb.append("\\f");
                    break;
                case '\r':
                    sb.append("\\r");
                    break;
                default:
                    if (c < ' ' || (c >= '\u0080' && c < '\u00a0')
                            || (c >= '\u2000' && c < '\u2100')) {
                        hhhh = "000" + Integer.toHexString(c);
                        sb.append("\\u").append(hhhh.substring(hhhh.length() - 4));
                    } else {
                        sb.append(c);
                    }
            }
        }
        sb.append('"');
        return sb.toString();
    }
}
