package es.tid.cosmos.base.data;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.commons.lang.ArrayUtils;
import org.apache.hadoop.io.WritableUtils;

import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;

/**
 *
 * @author ximo
 */
public class TypedProtobufWritable<M extends Message> extends ProtobufWritable<M> { 
    public TypedProtobufWritable() {
        super(null, null);
    }

    public TypedProtobufWritable(M obj) {
        super(obj, null);
        this.setConverter((Class) obj.getClass());
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
    public void readFields(DataInput in) throws IOException {
        String className = WritableUtils.readString(in);
        Class clazz;
        if (className.isEmpty()) {
            clazz = Message.class;
        } else {
            try {
                clazz = Class.forName(className);
            } catch (ClassNotFoundException ex) {
                throw new IllegalArgumentException("Byte stream does not have a "
                        + "valid class identifier", ex);
            }
        }
        this.setConverter(clazz);
        super.readFields(in);
    }
    
    @Override
    public void write(DataOutput out) throws IOException {
        M obj = this.get();
        if (obj == null) {
            WritableUtils.writeString(out, "");
        } else {
            WritableUtils.writeString(out, obj.getClass().getName());
        }
        super.write(out);
    }
}