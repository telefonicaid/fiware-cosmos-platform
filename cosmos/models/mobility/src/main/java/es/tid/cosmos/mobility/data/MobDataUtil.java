package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public abstract class MobDataUtil implements ProtobufUtil {
    public static MobData create(Cdr cdr) {
        return MobData.newBuilder().setCdr(cdr).build();
    }
    
    public static MobData create(Cell cell) {
        return MobData.newBuilder().setCell(cell).build();
    }
    
    public static ProtobufWritable<MobData> wrap(MobData obj) {
        ProtobufWritable<MobData> wrapper = ProtobufWritable.newInstance(
                MobData.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MobData> createAndWrap(Cdr obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Cell obj) {
        return wrap(create(obj));
    }
}
