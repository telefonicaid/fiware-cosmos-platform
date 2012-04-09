package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public abstract class TwoIntUtil implements ProtobufUtil {
    public static TwoInt create(long num1, long num2) {
        return TwoInt.newBuilder()
                .setNum1(num1)
                .setNum2(num2)
                .build();
    }

    public static ProtobufWritable<TwoInt> createAndWrap(long num1, long num2) {
        ProtobufWritable<TwoInt> wrapper =
                ProtobufWritable.newInstance(TwoInt.class);
        wrapper.set(create(num1, num2));
        return wrapper;
    }
}