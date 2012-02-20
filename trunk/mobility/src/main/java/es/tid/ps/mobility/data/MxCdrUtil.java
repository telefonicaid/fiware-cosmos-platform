package es.tid.ps.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.ps.mobility.data.BaseProtocol.Date;
import es.tid.ps.mobility.data.BaseProtocol.Time;
import es.tid.ps.mobility.data.MxProtocol.MxCdr;

/**
 *
 * @author dmicol
 */
public abstract class MxCdrUtil  implements ProtobufUtil {
    public static MxCdr create(long phone, long cell, Date date, Time time) {
        return MxCdr.newBuilder()
                .setPhone(phone)
                .setCell(cell)
                .setDate(date)
                .setTime(time)
                .build();
    }

    public static ProtobufWritable createAndWrap(long phone, long cell,
            Date date, Time time) {
        ProtobufWritable<MxProtocol.MxCdr> wrapper =
                ProtobufWritable.newInstance(MxCdr.class);
        wrapper.set(create(phone, cell, date, time));
        return wrapper;
    }
}
