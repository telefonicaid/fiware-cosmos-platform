package es.tid.cosmos.base.data;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

/**
 * 
 * @author dmicol
 */
public final class MessageDescriptor {
    private Map<String, String> fieldsMap;
    
    public MessageDescriptor() {
        this.fieldsMap = new HashMap<String, String>();
    }
    
    public MessageDescriptor(InputStream configInput) throws IOException {
        this();
        Properties props = new Properties();
        props.load(configInput);
        for (String propertyName : props.stringPropertyNames()) {
            this.set(propertyName,
                     props.getProperty(propertyName).toLowerCase());
        }
    }
    
    public String get(String fieldName) {
        return this.fieldsMap.get(fieldName);
    }
    
    public Set<String> getFieldNames() {
        return this.fieldsMap.keySet();
    }
    
    public void set(String fieldName, String fieldValue) {
        if (this.fieldsMap.containsKey(fieldName)) {
            throw new IllegalArgumentException("Repeated field " + fieldName);
        }
        this.fieldsMap.put(fieldName, fieldValue);
    }
}
