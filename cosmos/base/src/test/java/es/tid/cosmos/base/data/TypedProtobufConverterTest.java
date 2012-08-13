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

import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.cosmos.base.data.generated.BaseTypes.Int;

/**
 *
 * @author ximo
 */
public class TypedProtobufConverterTest {
    @Test
    public void testSerializeDeserialize() {
        TypedProtobufConverter<Int> converter = new TypedProtobufConverter<Int>();
        Int value = Int.newBuilder().setValue(125).build();
        Int processed = converter.fromBytes(converter.toBytes(value));
        assertEquals(value.getValue(), processed.getValue());
    }
}
