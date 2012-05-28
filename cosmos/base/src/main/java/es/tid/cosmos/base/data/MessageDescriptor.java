package es.tid.cosmos.base.data;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

/**
 * 
 * @author dmicol
 */
public final class MessageDescriptor {
    static final String TYPE_FIELD_NAME = "type";
    static final String DELIMITER_FIELD_NAME = "delimiter";
    
    private static final String[] META_FIELDS = { TYPE_FIELD_NAME,
                                                  DELIMITER_FIELD_NAME };
    
    private Map<String, String> metaInfo;
    private Map<String, Integer> fieldColumnIndices;
    
    public MessageDescriptor() {
        this.metaInfo = new HashMap<String, String>();
        this.fieldColumnIndices = new HashMap<String, Integer>();
    }
    
    public MessageDescriptor(InputStream configInput) throws IOException {
        this();
        Properties props = new Properties();
        props.load(configInput);
        for (String propertyName : props.stringPropertyNames()) {
            final String propertyValue = props.getProperty(propertyName);
            if (Arrays.binarySearch(META_FIELDS, propertyName) >= 0) {
                this.setMetaFieldValue(propertyName,
                                       propertyValue.toLowerCase());
            } else {
                this.setFieldColumnIndex(propertyName,
                                         Integer.parseInt(propertyValue));
            }
        }
    }
    
    public String getMetaFieldValue(String fieldName) {
        if (!this.metaInfo.containsKey(fieldName)) {
            throw new IllegalArgumentException("Not a meta field: "
                    + fieldName);
        }
        return this.metaInfo.get(fieldName);
    }

    public void setMetaFieldValue(String fieldName, String fieldValue) {
        if (this.metaInfo.containsKey(fieldName)) {
            throw new IllegalArgumentException("Repeated field " + fieldName);
        }
        this.metaInfo.put(fieldName, fieldValue);
    }
    
    public int getFieldColumnIndex(String fieldName) {
        if (!this.fieldColumnIndices.containsKey(fieldName)) {
            throw new IllegalArgumentException("Not a regular field: "
                    + fieldName);
        }
        return this.fieldColumnIndices.get(fieldName);
    }
    
    public Set<String> getFieldNames() {
        return this.fieldColumnIndices.keySet();
    }
    
    public void setFieldColumnIndex(String fieldName, int columnIndex) {
        if (this.fieldColumnIndices.containsKey(fieldName)) {
            throw new IllegalArgumentException("Repeated field " + fieldName);
        }
        this.fieldColumnIndices.put(fieldName, columnIndex);
    }
}
