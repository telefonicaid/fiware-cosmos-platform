package es.tid.cosmos.base.util;

import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author dmicol
 */
public class ArgumentParser {
    private Map<String, String> arguments;
    
    public ArgumentParser() {
    }
    
    public void parse(String[] args) {
        this.arguments = new HashMap<String, String>();
        for (String arg : args) {
            String[] fields = arg.split("=");
            String name = fields[0].replaceAll("--", "");
            String value;
            if (fields.length == 1) {
                value = "true";
            } else if (fields.length == 2) {
                value = fields[1];
            } else {
                throw new IllegalArgumentException(
                        "Invalid command line argument");
            }
            if (this.arguments.containsKey(name)) {
                throw new IllegalArgumentException(
                        "Found repeated command line argument: " + name);
            }
            this.arguments.put(name, value);
        }
    }

    public boolean has(String name) {
        return this.has(name, false);
    }
    
    public boolean has(String name, boolean required) {
        if (this.arguments.containsKey(name)) {
            return true;
        } else {
            if (required) {
                throw new IllegalArgumentException("Missing argument " + name);
            }
            return false;
        }
    }
    
    public boolean getBoolean(String name) {
        return this.getBoolean(name, false);
    }
    
    public boolean getBoolean(String name, boolean required) {
        if (!this.has(name, required)) {
            return false;
        }
        return Boolean.parseBoolean(this.arguments.get(name));
    }
    
    public String getString(String name) {
        return this.getString(name, false);
    }
    
    public String getString(String name, boolean required) {
        if (!this.has(name, required)) {
            return "";
        }
        return this.arguments.get(name);
    }
}
