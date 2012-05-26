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
    private static final String TYPE_FIELD_NAME = "type";
    private static final String ULER_TYPE_NAME = "uler";
    private static final String WEBLOG_TYPE_NAME = "weblog";
    
    private MessageGenerator() {
    }
    
    public static Message generate(MessageDescriptor messageDescriptor) {
        final String type = messageDescriptor.get(TYPE_FIELD_NAME);
        Builder builder;
        Descriptor descriptor;
        if (type.equals(ULER_TYPE_NAME)) {
            builder = Uler.newBuilder();
            descriptor = Uler.getDescriptor();
        } else if (type.equals(WEBLOG_TYPE_NAME)) {
            builder = WebLog.newBuilder();
            descriptor = WebLog.getDescriptor();
        } else {
            throw new IllegalArgumentException("Invalid type");
        }
        return setFields(builder, descriptor, messageDescriptor);
    }
    
    private static Message setFields(Builder builder, Descriptor descriptor,
                                     MessageDescriptor messageDescriptor) {
        for (String fieldName : messageDescriptor.getFieldNames()) {
            if (fieldName.equals(TYPE_FIELD_NAME)) {
                continue;
            }
            FieldDescriptor fieldDesc = descriptor.findFieldByName(fieldName);
            if (fieldDesc == null) {
                throw new IllegalArgumentException("Invalid field name: "
                        + fieldName);
            }
            builder.setField(fieldDesc,
                             cast(fieldDesc, messageDescriptor.get(fieldName)));
        }
        return builder.build();
    }
    
    private static Object cast(FieldDescriptor fieldDesc, String value) {
        switch (fieldDesc.getType()) {
            case BOOL:
                return Boolean.parseBoolean(value);
            case DOUBLE:
                return Double.parseDouble(value);
            case FIXED32:
            case INT32:
            case SINT32:
            case UINT32:
                return Integer.parseInt(value);
            case FIXED64:
            case INT64:
            case SINT64:
            case UINT64:
                return Long.parseLong(value);
            case FLOAT:
                return Float.parseFloat(value);
            case STRING:
                return value;
            default:
                throw new IllegalArgumentException("Invalid type for field "
                        + fieldDesc.getName());
        }
    }
}
