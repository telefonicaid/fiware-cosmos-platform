package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.MobProtocol.ItinTime;

/**
 *
 * @author dmicol
 */
public abstract class ItinTimeUtil {
    public static ItinTime create(Date date, Time time, long bts) {
        return ItinTime.newBuilder()
                .setDate(date)
                .setTime(time)
                .setBts(bts)
                .build();
    }

    public static ProtobufWritable<ItinTime> wrap(ItinTime obj) {
        ProtobufWritable<ItinTime> wrapper = ProtobufWritable.newInstance(
                ItinTime.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<ItinTime> createAndWrap(Date date, Time time,
                                                           long bts) {
        return wrap(create(date, time, bts));
    }
}
