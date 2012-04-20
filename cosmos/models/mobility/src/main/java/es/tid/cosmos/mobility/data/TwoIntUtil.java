package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public abstract class TwoIntUtil implements ProtobufUtil {
    private static final String DELIMITER = "|";
    
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
    
    public static TwoInt parse(String line) {
        String[] values = line.split("\\" + DELIMITER);
        return create(Long.parseLong(values[0]), Long.parseLong(values[1]));
    }
    
    public static String toString(TwoInt obj) {
        return (obj.getNum1() + DELIMITER + obj.getNum2());
    }
}
