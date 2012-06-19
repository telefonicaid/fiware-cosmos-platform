package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public final class TwoIntUtil {

    private TwoIntUtil() {}

    public static TwoInt create(long num1, long num2) {
        return TwoInt.newBuilder()
                .setNum1(num1)
                .setNum2(num2)
                .build();
    }
    
    public static ProtobufWritable<TwoInt> wrap(TwoInt obj) {
        ProtobufWritable<TwoInt> wrapper =
                ProtobufWritable.newInstance(TwoInt.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<TwoInt> createAndWrap(long num1, long num2) {
        return wrap(create(num1, num2));
    }
    
    public static long getPartition(TwoInt obj, long maxNumPartitions) {
        return (obj.getNum1() % maxNumPartitions);
    }
    
    public static String toString(TwoInt obj, String separator) {
        return (obj.getNum1() + separator + obj.getNum2());
    }
}
