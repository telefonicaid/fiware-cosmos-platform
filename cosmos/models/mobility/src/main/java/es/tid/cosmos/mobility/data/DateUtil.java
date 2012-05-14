package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;

/**
 *
 * @author sortega
 */
public abstract class DateUtil {
    public static final String DELIMITER = "|";
    
    public static Date create(int year, int month, int day, int weekday) {
        return Date.newBuilder()
                .setYear(year)
                .setMonth(month)
                .setDay(day)
                .setWeekday(weekday)
                .build();
    }

    public static ProtobufWritable<Date> wrap(Date obj) {
        ProtobufWritable<Date> wrapper =
                ProtobufWritable.newInstance(Date.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<Date> createAndWrap(int year, int month,
                                                       int day, int weekday) {
        return wrap(create(year, month, day, weekday));
    }
    
    public static Date parse(String line) {
        String[] values = line.split("\\" + DELIMITER);
        return create(Integer.parseInt(values[0]), Integer.parseInt(values[1]),
                      Integer.parseInt(values[2]), Integer.parseInt(values[3]));
    }
    
    public static String toString(Date obj) {
        return (obj.getYear() + DELIMITER + obj.getMonth() + DELIMITER +
                obj.getDay() + DELIMITER + obj.getWeekday());
    }
}
