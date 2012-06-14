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
