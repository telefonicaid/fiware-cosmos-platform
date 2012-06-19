package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;

/**
 *
 * @author sortega
 */
public final class BtsCounterUtil {

    private BtsCounterUtil() {}

    public static BtsCounter create(long bts, int weekday, int range,
            int count) {
        return BtsCounter.newBuilder()
                .setBts(bts)
                .setWeekday(weekday)
                .setRange(range)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<BtsCounter> wrap(BtsCounter obj) {
        ProtobufWritable<BtsCounter> wrapper =
                ProtobufWritable.newInstance(BtsCounter.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<BtsCounter> createAndWrap(long bts,
            int weekday, int range, int count) {
        return wrap(create(bts, weekday, range, count));
    }
    
    public static String toString(BtsCounter obj, String separator) {
        return (obj.getBts() + separator + obj.getWeekday() + separator +
                obj.getRange() + separator + obj.getCount());
    }
}
