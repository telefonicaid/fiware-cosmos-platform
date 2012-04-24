package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.parsing.CdrParser;

/**
 *
 * @author dmicol
 */
public abstract class CdrUtil {
    public static Cdr create(long userId, long cellId, Date date, Time time) {
        return Cdr.newBuilder()
                .setUserId(userId)
                .setCellId(cellId)
                .setDate(date)
                .setTime(time)
                .build();
    }

    public static ProtobufWritable<Cdr> wrap(Cdr obj) {
        ProtobufWritable<Cdr> wrapper = ProtobufWritable.newInstance(Cdr.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable createAndWrap(long userId, long cellId,
                                                 Date date, Time time) {
        return wrap(create(userId, cellId, date, time));
    }
    
    public static Cdr parse(String line) {
        return new CdrParser(line).parse();
    }
}
