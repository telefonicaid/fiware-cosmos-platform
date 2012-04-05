package es.tid.bdp.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.mobility.data.MobProtocol.BtsCounter;

/**
 *
 * @author sortega
 */
public abstract class BtsCounterUtil implements ProtobufUtil {
    public static BtsCounter create(long placeId, int weekday, int range,
            int count) {
        return BtsCounter.newBuilder()
                .setPlaceId(placeId)
                .setWeekday(weekday)
                .setRange(range)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<BtsCounter> createAndWrap(long placeId,
            int weekday, int range, int count) {
        ProtobufWritable<BtsCounter> wrapper =
                ProtobufWritable.newInstance(BtsCounter.class);
        wrapper.set(create(placeId, weekday, range, count));
        return wrapper;
    }
}