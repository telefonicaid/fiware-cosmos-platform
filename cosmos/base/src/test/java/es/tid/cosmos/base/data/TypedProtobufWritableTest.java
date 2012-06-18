package es.tid.cosmos.base.data;

import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;

/**
 *
 * @author ximo
 */
public class TypedProtobufWritableTest {
    @Test
    public void testCreators() {
        TypedProtobufWritable<Int> intObj = TypedProtobufWritable.create(4);
        assertEquals(intObj.get().getValue(), 4);

        TypedProtobufWritable<Int64> longObj = TypedProtobufWritable.create(5L);
        assertEquals(longObj.get().getValue(), 5L);

        TypedProtobufWritable<Float64> doubleObj = TypedProtobufWritable.create(
                3.5D);
        assertEquals(doubleObj.get().getValue(), 3.5D, 0.0D);
    }

    @Test
    public void testGroupByClass() {
        List<TypedProtobufWritable<Message>> list =
                new LinkedList<TypedProtobufWritable<Message>>();
        list.add(new TypedProtobufWritable<Message>(Int.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(Int64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(Int64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(
                Float64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(
                Float64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(
                Float64.getDefaultInstance()));
        Map<Class, List> groups = TypedProtobufWritable.groupByClass(list,
                Int.class, Int64.class, Float64.class, Message.class);
        assertEquals(groups.get(Int.class).size(), 1);
        assertEquals(groups.get(Int64.class).size(), 2);
        assertEquals(groups.get(Float64.class).size(), 3);
        assertEquals(groups.get(Message.class).size(), 0);
    }
    
    @Test(expected=IllegalStateException.class)
    public void testGroupByClassFail() {
        List<TypedProtobufWritable<Message>> list =
                new LinkedList<TypedProtobufWritable<Message>>();
        list.add(new TypedProtobufWritable<Message>(Int.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(Int64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(Int64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(
                Float64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(
                Float64.getDefaultInstance()));
        list.add(new TypedProtobufWritable<Message>(
                Float64.getDefaultInstance()));
        Map<Class, List> groups = TypedProtobufWritable.groupByClass(list,
                Int.class, Int64.class);
    }
}
