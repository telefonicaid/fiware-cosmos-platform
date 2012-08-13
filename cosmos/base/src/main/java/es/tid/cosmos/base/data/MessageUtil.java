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
