/**
 * \file Options.java
 * \author Philippe H. Gosselin
 * \version 4.0
 */
package retin.toolbox.document;

import java.io.IOException;
import java.util.*;
import retin.toolbox.document.serialization.JsonWriter;
import retin.toolbox.document.serialization.Jsonable;

public class Options implements Jsonable {

    public abstract class Option implements Jsonable {

        String name;
        boolean bSubOptions;

        public Option(String name) {
            this.name = name;
        }

        public String getName() {
            return name;
        }

        public boolean hasSubOptions() {
            return bSubOptions;
        }

        public abstract void setValueToDefault();

        public abstract void setParameter(Parameter p);

        public abstract Parameter getParameter();
    };

    public class IntegerValue extends Option {

        int value, defaut;
        int min, max;

        public IntegerValue(String name) {
            super(name);
            min = value = defaut = 0;
            max = 100;
        }

        public IntegerValue setRange(int min, int max) {
            this.min = min;
            this.max = max;
            return this;
        }

        public IntegerValue setDefault(int defaut) {
            this.value = defaut;
            this.defaut = defaut;
            return this;
        }

        public void setValueToDefault() {
            value = defaut;
        }

        public void setParameter(Parameter p) {
            value = Integer.parseInt(p.getValue());
            if (value < min || value > max) {
                throw new IllegalArgumentException("A number between " + min + " and " + max + " is expected");
            }
        }

        public Parameter getParameter() {
            return new Parameter(Integer.toString(value));
        }

        public void toJsonValue(JsonWriter out) throws IOException {
            out.object();
            out.key("name").value(name);
            out.key("type").value("IntegerValue");
            out.key("min").value(min);
            out.key("max").value(max);
            out.key("defaut").value(defaut);
            out.endObject();
        }
    }

    public class DoubleValue extends Option {

        double value, defaut;
        double min, max;

        public DoubleValue(String name) {
            super(name);
            min = value = defaut = 0;
            max = 100;
        }

        public DoubleValue setRange(double min, double max) {
            this.min = min;
            this.max = max;
            return this;
        }

        public DoubleValue setDefault(double defaut) {
            this.value = defaut;
            this.defaut = defaut;
            return this;
        }

        public void setValueToDefault() {
            value = defaut;
        }

        public void setParameter(Parameter p) {
            this.value = Double.parseDouble(p.getValue());
            if (value < min || value > max) {
                throw new IllegalArgumentException("A number between " + min + " and " + max + " is expected");
            }
        }

        public Parameter getParameter() {
            return new Parameter(Double.toString(value));
        }

        public void toJsonValue(JsonWriter out) throws IOException {
            out.object();
            out.key("name").value(name);
            out.key("type").value("DoubleValue");
            out.key("min").value(min);
            out.key("max").value(max);
            out.key("defaut").value(defaut);
            out.endObject();
        }
    }

    public class StringValue extends Option {

        String value, defaut;
        boolean longText;

        public StringValue(String name) {
            super(name);
        }

        public StringValue setLongText(boolean longText) {
            this.longText = longText;
            return this;
        }

        public boolean isLongText() {
            return longText;
        }

        public StringValue setDefault(String defaut) {
            this.value = defaut;
            this.defaut = defaut;
            return this;
        }

        public void setValueToDefault() {
            value = defaut;
        }

        public void setParameter(Parameter p) {
            this.value = p.getValue();
        }

        public Parameter getParameter() {
            return new Parameter(value);
        }

        public void toJsonValue(JsonWriter out) throws IOException {
            out.object();
            out.key("name").value(name);
            out.key("type").value("StringValue");
            out.key("defaut").value(defaut);
            out.endObject();
        }
    }

    public class DirOpen extends StringValue {

        public DirOpen(String name) {
            super(name);
        }
    }

    public class FileOpen extends StringValue {

        public FileOpen(String name) {
            super(name);
        }
    }

    public class FileCreate extends StringValue {

        public FileCreate(String name) {
            super(name);
        }
    }

    public class StringValues extends Option {

        ArrayList<String> defaut;
        ArrayList<String> values;

        public StringValues(String name) {
            super(name);
            defaut = new ArrayList<String>();
            values = new ArrayList<String>();
        }

        public StringValues addDefault(String value) {
            if (defaut.indexOf(value) < 0) {
                defaut.add(value);
                values.add(value);
            }
            return this;
        }

        public StringValues addDefaults(String[] values) {
            for (int i = 0; i < values.length; i++) {
                addDefault(values[i]);
            }
            return this;
        }

        public void setValueToDefault() {
            values = defaut;
        }

        public int getValueCount() {
            return values.size();
        }

        public String getValue(int i) {
            return values.get(i);
        }

        public String[] getValues() {
            String[] array = values.toArray(new String[0]);
            java.util.Arrays.sort(array);
            return array;
        }

        public boolean addValue(String value) {
            if (values.indexOf(value) >= 0) {
                return false;
            }
            values.add(value);
            return true;
        }

        public boolean remValue(String value) {
            return values.remove(value);
        }

        public void setParameter(Parameter p) {
            values.clear();
            String[] array = p.getParams().getNames();
            values.addAll(Arrays.asList(array));
        }

        public Parameter getParameter() {
            Parameters params = new Parameters();
            for (int i = 0; i < values.size(); i++) {
                params.set(values.get(i), null);
            }
            return new Parameter(null, params);
        }

        public void toJsonValue(JsonWriter out) throws IOException {
            out.object();
            out.key("name").value(name);
            out.key("type").value("StringValues");
            out.key("defauts").value(defaut);
            out.endObject();
        }
    }

    public class StringSelection extends Option {

        String defaultValue;
        String selectedValue;
        TreeMap<String, Options> values;
        boolean editable;

        public StringSelection(String name) {
            super(name);
            values = new TreeMap<String, Options>();
        }

        public StringSelection setEditable(boolean b) {
            editable = b;
            return this;
        }

        public boolean isEditable() {
            return editable;
        }

        public StringSelection add(String v) {
            if (defaultValue == null) {
                setDefault(v);
            }
            values.put(v, null);
            return this;
        }

        public StringSelection add(String v, Options options) {
            if (options == null) {
                return add(v);
            }
            if (defaultValue == null) {
                setDefault(v);
            }
            values.put(v, options);
            bSubOptions = true;
            return this;
        }

        public StringSelection add(String[] v) {
            for (int i = 0; i < v.length; i++) {
                add(v[i]);
            }
            return this;
        }

        public StringSelection add(String[] v, Options[] options) {
            for (int i = 0; i < v.length; i++) {
                add(v[i], options[i]);
            }
            return this;
        }

        public StringSelection setDefault(String v) {
            selectedValue = v;
            defaultValue = v;
            return this;
        }

        public void setValueToDefault() {
            selectedValue = defaultValue;
            java.util.Iterator<Options> ite = values.values().iterator();
            while (ite.hasNext()) {
                Options options = ite.next();
                if (options != null) {
                    options.setValuesToDefault();
                }
            }
        }

        public String getSelectedValue() {
            return selectedValue;
        }

        public void setSelectedValue(String value) {
            selectedValue = value;
        }

        public String[] getValues() {
            String[] array = values.keySet().toArray(new String[0]);
            java.util.Arrays.sort(array);
            return array;
        }

        public Options getSubOptions(String value) {
            return values.get(value);
        }

        public void setParameter(Parameter p) {
            selectedValue = p.getValue();
            values.get(selectedValue).setParameters(p.getParams());
        }

        public Parameter getParameter() {
            if (selectedValue == null) {
                throw new IllegalArgumentException("No value");
            }
            if (values.get(selectedValue) == null) {
                return new Parameter(selectedValue);
            }
            return new Parameter(selectedValue, values.get(selectedValue).getParameters());
        }

        public void toJsonValue(JsonWriter out) throws IOException {
            out.object();
            out.key("name").value(name);
            out.key("type").value("StringSelection");
            out.key("defaut").value(defaultValue);
            out.key("values").value(values);
            out.endObject();
        }
    }

    public class MultipleStringSelection extends Option {

        HashSet<String> defaultValues;
        HashSet<String> selectedValues;
        HashMap<String, Options> values;

        public MultipleStringSelection(String name) {
            super(name);
            values = new HashMap<String, Options>();
            selectedValues = new HashSet<String>();
            defaultValues = new HashSet<String>();
        }

        public MultipleStringSelection add(String v) {
            values.put(v, null);
            return this;
        }

        public MultipleStringSelection add(String v, Options options) {
            values.put(v, options);
            bSubOptions = true;
            return this;
        }

        public MultipleStringSelection add(String[] v) {
            for (int i = 0; i < v.length; i++) {
                add(v[i]);
            }
            return this;
        }

        public MultipleStringSelection addDefault(String v) {
            if (values.containsKey(v)) {
                selectedValues.add(v);
                defaultValues.add(v);
            }
            return this;
        }

        public MultipleStringSelection addDefault(String[] v) {
            for (int i = 0; i < v.length; i++) {
                addDefault(v[i]);
            }
            return this;
        }

        public void setValueToDefault() {
            selectedValues = defaultValues;
            java.util.Iterator<Options> ite = values.values().iterator();
            while (ite.hasNext()) {
                Options options = ite.next();
                if (options != null) {
                    options.setValuesToDefault();
                }
            }
        }

        public String[] getValues() {
            String[] array = values.keySet().toArray(new String[0]);
            java.util.Arrays.sort(array);
            return array;
        }

        public String[] getSelectedValues() {
            return selectedValues.toArray(new String[0]);
        }

        public boolean isSelectedValue(String value) {
            return selectedValues.contains(value);
        }

        public void selectValue(String value) {
            selectedValues.add(value);
        }

        public void unselectValue(String value) {
            selectedValues.remove(value);
        }

        public Options getSubOptions(String value) {
            return values.get(value);
        }

        public void setParameter(Parameter p) {
            selectedValues = new HashSet<String>();
            if (p == null) {
                return;
            }
            Parameters selected = p.getParams();
            if (selected == null) {
                return;
            }
            String[] names = selected.getNames();
            for (int i = 0; i < names.length; i++) {
                selectedValues.add(names[i]);
                Options subOptions = values.get(names[i]);
                if (subOptions != null) {
                    subOptions.setParameters(selected.getParameter(names[i]).getParams());
                }
            }
        }

        public Parameter getParameter() {
            Parameters p = new Parameters();
            String[] selected = getSelectedValues();
            for (int i = 0; i < selected.length; i++) {
                Options subOptions = values.get(selected[i]);
                if (subOptions == null) {
                    p.set(selected[i], null);
                } else {
                    p.set(selected[i], null, subOptions.getParameters());
                }
            }
            return new Parameter(null, p);
        }

        public void toJsonValue(JsonWriter out) throws IOException {
            out.object();
            out.key("name").value(name);
            out.key("type").value("MultipleStringSelection");
            out.key("defauts").value(defaultValues);
            out.key("values").value(values);
            out.endObject();
        }
    }

    public class ParametersValue extends Option {

        Parameters defaut, value;

        public ParametersValue(String name) {
            super(name);
            defaut = new Parameters();
            value = new Parameters();
        }

        public ParametersValue setDefault(Parameters params) {
            defaut = params;
            value = params.copy();
            return this;
        }

        public void setValueToDefault() {
            value = defaut.copy();
        }

        public Parameters getParameters() {
            return value;
        }

        public void setParameter(Parameter p) {
            value = p.getParams().copy();
        }

        public Parameter getParameter() {
            return new Parameter(null, value);
        }

        public void toJsonValue(JsonWriter out) throws IOException {
            throw new UnsupportedOperationException("Not supported yet.");
        }
    }
    ArrayList<Option> options;

    public Options() {
        options = new ArrayList<Option>();
    }

    public Option[] getOptions() {
        return options.toArray(new Option[0]);
    }

    public Option getOption(String name) {
        for (int i = 0; i < options.size(); i++) {
            if (options.get(i).getName().equals(name)) {
                return options.get(i);
            }
        }
        return null;
    }

    public void setValuesToDefault() {
        for (int i = 0; i < options.size(); i++) {
            options.get(i).setValueToDefault();
        }
    }

    public void setParameters(Parameters params) {
        for (int i = 0; i < options.size(); i++) {
            Option option = options.get(i);
            if (params != null && params.has(option.getName())) {
                option.setParameter(params.getParameter(option.getName()));
            } else {
                option.setValueToDefault();
            }
        }
    }

    public Parameters getParameters() {
        Parameters params = new Parameters();
        for (int i = 0; i < options.size(); i++) {
            Option option = options.get(i);
//System.out.println(option.getName());
            params.setParameter(option.getName(), option.getParameter());
        }
        return params;
    }

    public IntegerValue addInteger(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        IntegerValue option = new IntegerValue(name);
        options.add(option);
        return option;
    }

    public DoubleValue addDouble(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        DoubleValue option = new DoubleValue(name);
        options.add(option);
        return option;
    }

    public StringValue addString(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        StringValue option = new StringValue(name);
        options.add(option);
        return option;
    }

    public DirOpen addDirOpen(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        DirOpen option = new DirOpen(name);
        options.add(option);
        return option;
    }

    public FileOpen addFileOpen(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        FileOpen option = new FileOpen(name);
        options.add(option);
        return option;
    }

    public FileCreate addFileCreate(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        FileCreate option = new FileCreate(name);
        options.add(option);
        return option;
    }

    public StringValues addStringValues(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        StringValues option = new StringValues(name);
        options.add(option);
        return option;
    }

    public StringSelection addStringSelection(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        StringSelection option = new StringSelection(name);
        options.add(option);
        return option;
    }

    public MultipleStringSelection addMultipleStringSelection(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        MultipleStringSelection option = new MultipleStringSelection(name);
        options.add(option);
        return option;
    }

    public ParametersValue addParametersValue(String name) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        ParametersValue option = new ParametersValue(name);
        options.add(option);
        return option;
    }

    public ParametersValue addParametersValue(String name, Parameters params) {
        if (getOption(name) != null) {
            System.out.println("L'option " + name + " existe déjà");
        }
        ParametersValue option = new ParametersValue(name);
        options.add(option);
        if (params.hasParams(name)) {
            try {
                option.setDefault(params.getParams(name));
            } catch (Exception e) {
            }
        }
        return option;
    }

    public void toJsonValue(JsonWriter out) throws IOException {
        out.value(options);
    }
}
