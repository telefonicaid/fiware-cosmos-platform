package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.BaseProtocol.Time;

/**
 *
 * @author sortega
 */
public abstract class TimeUtil implements ProtobufUtil {
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
    
    public static Time parse(String line) {
        String[] values = line.split(DELIMITER);
        return create(Integer.parseInt(values[0]), Integer.parseInt(values[1]),
                      Integer.parseInt(values[2]));
    }
    
    public static String toString(Time obj) {
        return (obj.getHour() + DELIMITER + obj.getMinute() + DELIMITER +
                obj.getSeconds());
    }
}
