package es.tid.cosmos.base.data;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.BinaryConverter;
import com.twitter.elephantbird.mapreduce.io.BinaryWritable;
import org.apache.commons.lang.ArrayUtils;

import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;

/**
 *
 * @author ximo
 */
public class TypedProtobufWritable<M extends Message> extends BinaryWritable<M> {
    public TypedProtobufWritable() {
        super(null, new TypedProtobufConverter<M>());
    }

    public TypedProtobufWritable(M obj) {
        super(obj, new TypedProtobufConverter<M>());
    }

    public static Map<Class, List> groupByClass(
            Iterable<TypedProtobufWritable<Message>> values,
            Class<? extends Message>... classes) {
        Map<Class, List> ret = new HashMap<Class, List>();
        for (Class c : classes) {
            ret.put(c, new LinkedList());
        }
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            final Class clazz = message.getClass();
            if (!ArrayUtils.contains(classes, clazz)) {
                throw new IllegalStateException("Unexpected Message: "
                            + message.toString());
            }
            ret.get(clazz).add(message);
        }
        return ret;
    }

    public static TypedProtobufWritable<Float64> create(double d) {
        return new TypedProtobufWritable<Float64>(
                Float64.newBuilder().setValue(d).build());
    }

    public static TypedProtobufWritable<Int> create(int d) {
        return new TypedProtobufWritable<Int>(
                Int.newBuilder().setValue(d).build());
    }

    public static TypedProtobufWritable<Int64> create(long d) {
        return new TypedProtobufWritable<Int64>(
                Int64.newBuilder().setValue(d).build());
    }

    @Override
    protected BinaryConverter<M> getConverterFor(Class<M> clazz) {
        return new TypedProtobufConverter();
    }

    public static<T extends Message> List<TypedProtobufWritable<T>>
            asList(T... messages) {
        List<TypedProtobufWritable<T>> protobufList =
                new LinkedList<TypedProtobufWritable<T>>();
        for (T message : messages) {
            protobufList.add(new TypedProtobufWritable<T>(message));
        }
        return protobufList;
    }
}
