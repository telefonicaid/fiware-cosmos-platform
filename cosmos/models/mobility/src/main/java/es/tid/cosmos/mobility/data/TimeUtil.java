package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;

/**
 *
 * @author sortega
 */
public abstract class TimeUtil {
    private static final String DELIMITER = "|";
    
    public static Time create(int hour, int minute, int seconds) {
        return Time.newBuilder()
                .setHour(hour)
                .setMinute(minute)
                .setSeconds(seconds)
                .build();
    }

    public static ProtobufWritable<Time> wrap(Time obj) {
        ProtobufWritable<Time> wrapper =
                ProtobufWritable.newInstance(Time.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<Time> createAndWrap(int hour, int minute,
                                                       int seconds) {
        return wrap(create(hour, minute, seconds));
    }
    
    public static String toString(Time obj) {
        return (obj.getHour() + DELIMITER + obj.getMinute() + DELIMITER +
                obj.getSeconds());
    }
}
