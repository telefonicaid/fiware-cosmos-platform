package es.tid.cosmos.base.data;

import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Message;
import com.google.protobuf.Message.Builder;

import es.tid.cosmos.base.data.generated.CdrProtocol.Uler;
import es.tid.cosmos.base.data.generated.WebLogProtocol.WebLog;

/**
 * 
 * @author dmicol
 */
public abstract class MessageGenerator {
    private static final String ULER_TYPE_NAME = "uler";
    private static final String WEBLOG_TYPE_NAME = "weblog";
    
    private MessageGenerator() {
    }
    
    public static Message generate(MessageDescriptor messageDescriptor,
                                   String line) {
        final String type = messageDescriptor.getMetaFieldValue(
                MessageDescriptor.MetaFields.TYPE);
        final String delimiter = messageDescriptor.getMetaFieldValue(
                MessageDescriptor.MetaFields.DELIMITER);
        String[] fields = line.split(delimiter);
        Builder builder;
        Descriptor descriptor;
        if (type.equals(ULER_TYPE_NAME)) {
            builder = Uler.newBuilder();
            descriptor = Uler.getDescriptor();
        } else if (type.equals(WEBLOG_TYPE_NAME)) {
            builder = WebLog.newBuilder();
            descriptor = WebLog.getDescriptor();
        } else {
            throw new IllegalArgumentException("Invalid type: " + type);
        }
        return setFields(builder, descriptor, messageDescriptor, fields);
    }
    
    private static Message setFields(Builder builder, Descriptor descriptor,
                                     MessageDescriptor messageDescriptor,
                                     String[] fields) {
        for (String fieldName : messageDescriptor.getFieldNames()) {
            FieldDescriptor fieldDesc = descriptor.findFieldByName(fieldName);
            if (fieldDesc == null) {
                throw new IllegalArgumentException("Invalid field name: "
                        + fieldName);
            }
            String fieldValue;
            try {
                fieldValue = fields[messageDescriptor.getFieldColumnIndex(
                        fieldName)];
            } catch (IndexOutOfBoundsException ex) {
                throw new IllegalArgumentException("Invalid column index for "
                        + "field " + fieldName, ex);
            }
            builder.setField(fieldDesc, fieldValue);
        }
        return builder.build();
    }
}
