package es.tid.cosmos.mobility.data;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.BinaryConverter;
import com.twitter.elephantbird.mapreduce.io.BinaryWritable;
import org.apache.commons.lang.ArrayUtils;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Float64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;

/**
 *
 * @author ximo
 */
public class MobilityWritable<M extends Message> extends BinaryWritable<M> {    
    public static Map<Class, List> divideIntoTypes(
            Iterable<MobilityWritable<Message>> values,
            Class<? extends Message>... classes) {
        Map<Class, List> ret = new HashMap<Class, List>();
        for (MobilityWritable<Message> value : values) {
            final Message message = value.get();
            final Class clazz = message.getClass();
            if (!ArrayUtils.contains(classes, clazz)) {
                throw new IllegalStateException("Unexpected Message: "
                            + message.toString());
            }
            if (!ret.containsKey(clazz)) {
                ret.put(clazz, new LinkedList());
            }
            ret.get(clazz).add(message);
        }
        return ret;
    }
    
    public static MobilityWritable<Float64> create(double d) {
        return new MobilityWritable<Float64>(
                Float64.newBuilder().setNum(d).build());
    }
    
    public static MobilityWritable<Int> create(int d) {
        return new MobilityWritable<Int>(
                Int.newBuilder().setNum(d).build());
    }
    
    public static MobilityWritable<Int64> create(long d) {
        return new MobilityWritable<Int64>(
                Int64.newBuilder().setNum(d).build());
    }

    public MobilityWritable() {
        super(null, null);
    }

    public MobilityWritable(M obj) {
        super(obj, null);
    }

    @Override
    protected BinaryConverter<M> getConverterFor(Class<M> clazz) {
        return new MobilityConverter();
    }
}
