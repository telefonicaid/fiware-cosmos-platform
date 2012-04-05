package es.tid.bdp.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.mobility.data.BaseProtocol.Date;
import es.tid.bdp.mobility.data.BaseProtocol.Time;
import es.tid.bdp.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public abstract class CdrUtil  implements ProtobufUtil {
    public static Cdr create(long userId, long cellId, Date date, Time time) {
        return Cdr.newBuilder()
                .setUserId(userId)
                .setCellId(cellId)
                .setDate(date)
                .setTime(time)
                .build();
    }

    public static ProtobufWritable createAndWrap(long phone, long cell,
            Date date, Time time) {
        ProtobufWritable<Cdr> wrapper =
                ProtobufWritable.newInstance(Cdr.class);
        wrapper.set(create(phone, cell, date, time));
        return wrapper;
    }
}
