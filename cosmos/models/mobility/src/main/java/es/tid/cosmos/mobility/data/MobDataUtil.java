package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.*;

/**
 *
 * @author dmicol
 */
public abstract class MobDataUtil implements ProtobufUtil {
    public static MobData create(int value) {
        return MobData.newBuilder()
                .setType(MobData.Type.INT)
                .setInt(value)
                .build();
    }
    
    public static MobData create(Cdr cdr) {
        return MobData.newBuilder()
                .setType(MobData.Type.CDR)
                .setCdr(cdr)
                .build();
    }
    
    public static MobData create(Cell cell) {
        return MobData.newBuilder()
                .setType(MobData.Type.CELL)
                .setCell(cell)
                .build();
    }

    public static MobData create(Bts bts) {
        return MobData.newBuilder()
                .setBts(bts)
                .setType(MobData.Type.BTS)
                .build();
    }

    public static MobData create(Cluster cluster) {
        return MobData.newBuilder()
                .setType(MobData.Type.CLUSTER)
                .setCluster(cluster)
                .build();
    }

    public static MobData create(NodeBtsDay nodeBtsDay) {
        return MobData.newBuilder()
                .setType(MobData.Type.NODE_BTS_DAY)
                .setNodeBtsDay(nodeBtsDay)
                .build();
    }
    
    public static ProtobufWritable<MobData> wrap(MobData obj) {
        ProtobufWritable<MobData> wrapper = ProtobufWritable.newInstance(
                MobData.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MobData> createAndWrap(int obj) {
        return wrap(create(obj));
    }
    
    public static ProtobufWritable<MobData> createAndWrap(Cdr obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Cell obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Bts obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Cluster obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(NodeBtsDay obj) {
        return wrap(create(obj));
    }    
}
