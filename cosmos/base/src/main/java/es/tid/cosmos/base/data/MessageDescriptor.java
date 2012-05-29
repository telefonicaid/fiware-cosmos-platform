package es.tid.cosmos.base.data;

import java.io.IOException;
import java.io.InputStream;
import java.util.*;

/**
 * 
 * @author dmicol
 */
public final class MessageDescriptor {
    public enum MetaFields {
        TYPE,
        DELIMITER
    }
    
    private Map<MetaFields, String> metaInfo;
    private Map<String, Integer> fieldColumnIndices;
    
    public MessageDescriptor() {
        this.metaInfo = new EnumMap<MetaFields, String>(MetaFields.class);
        this.fieldColumnIndices = new HashMap<String, Integer>();
    }
    
    public MessageDescriptor(InputStream configInput) throws IOException {
        this();
        Properties props = new Properties();
        props.load(configInput);
        for (String propertyName : props.stringPropertyNames()) {
            final String normalizedPropertyName = propertyName.toUpperCase();
            final String propertyValue = props.getProperty(propertyName);
            if (Arrays.binarySearch(MetaFields.values(),
                                    normalizedPropertyName) >= 0) {
                this.setMetaFieldValue(
                        MetaFields.valueOf(normalizedPropertyName),
                        propertyValue.toLowerCase());
            } else {
                this.setFieldColumnIndex(propertyName,
                                         Integer.parseInt(propertyValue));
            }
        }
    }
    
    public String getMetaFieldValue(MetaFields metaField) {
        if (!this.metaInfo.containsKey(metaField)) {
            throw new IllegalArgumentException("Not a meta field: "
                    + metaField.name());
        }
        return this.metaInfo.get(metaField);
    }

    public void setMetaFieldValue(MetaFields metaField, String fieldValue) {
        if (this.metaInfo.containsKey(metaField)) {
            throw new IllegalArgumentException("Repeated field "
                    + metaField.name());
        }
        this.metaInfo.put(metaField, fieldValue);
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
