/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

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
        DELIMITER,
        ANONYMISE
    }

    private Map<MetaFields, String> metaInfo;
    private Set<String> fieldsToAnonymise;
    private Map<String, Integer> fieldColumnIndices;

    public MessageDescriptor() {
        this.metaInfo = new EnumMap<MetaFields, String>(MetaFields.class);
        this.fieldsToAnonymise = new HashSet<String>();
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
        this.checkForConsistency();
    }

    private void checkForConsistency() {
        for (String fieldToAnonymise : this.fieldsToAnonymise) {
            if (!this.fieldColumnIndices.containsKey(fieldToAnonymise)) {
                throw new IllegalArgumentException("Invalid field to "
                        + "anonymise: " + fieldToAnonymise);
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
        switch (metaField) {
            case TYPE:
                this.metaInfo.put(metaField, fieldValue);
                break;
            case DELIMITER:
                this.metaInfo.put(metaField, fieldValue.replace("\\\\", "\\"));
                break;
            case ANONYMISE:
                this.fieldsToAnonymise.addAll(Arrays.asList(
                        fieldValue.split(",")));
                break;
            default:
                throw new IllegalArgumentException("Unsupported metadata type");
        }
    }

    public boolean shouldAnonymiseField(String fieldName) {
        return this.fieldsToAnonymise.contains(fieldName);
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
