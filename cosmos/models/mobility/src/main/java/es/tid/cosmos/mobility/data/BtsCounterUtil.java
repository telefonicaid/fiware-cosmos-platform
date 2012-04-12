package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;

/**
 *
 * @author sortega
 */
public abstract class BtsCounterUtil implements ProtobufUtil {
    private static final String DELIMITER = "|";
    
    public static BtsCounter create(long placeId, int weekday, int range,
            int count) {
        return BtsCounter.newBuilder()
                .setPlaceId(placeId)
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
    
    public static ProtobufWritable<BtsCounter> createAndWrap(long placeId,
            int weekday, int range, int count) {
        return wrap(create(placeId, weekday, range, count));
    }
    
    public static BtsCounter parse(String line) {
        String[] values = line.split(DELIMITER);
        return create(Long.parseLong(values[0]), Integer.parseInt(values[1]),
                      Integer.parseInt(values[2]), Integer.parseInt(values[3]));
    }
    
    public static String toString(BtsCounter obj) {
        return (obj.getPlaceId() + DELIMITER + obj.getWeekday() + DELIMITER +
                obj.getRange() + DELIMITER + obj.getCount());
    }
}
