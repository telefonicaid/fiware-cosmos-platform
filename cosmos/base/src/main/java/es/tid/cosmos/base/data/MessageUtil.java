package es.tid.cosmos.base.data;

import com.google.protobuf.Message;

import es.tid.cosmos.base.data.MessageDescriptor.MetaFields;

/**
 *
 * @author dmicol
 */
public final class MessageUtil {
    private MessageUtil() {
    }

    public static String toString(MessageDescriptor messageDescriptor,
                                  Message message) {
        final String delimiter = messageDescriptor.getMetaFieldValue(
                MetaFields.DELIMITER);
        String output = "";
        for (Object fieldValue : message.getAllFields().values()) {
            if (!output.isEmpty()) {
                output += delimiter;
            }
            output += (String)fieldValue;
        }
        return output;
    }
}
