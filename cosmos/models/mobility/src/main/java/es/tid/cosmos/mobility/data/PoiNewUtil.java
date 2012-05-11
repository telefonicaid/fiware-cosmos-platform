package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;

/**
 *
 * @author dmicol
 */
public abstract class PoiNewUtil {
    public static PoiNew create(int id, long node, long bts,
            int labelgroupnodebts, int confidentnodebts) {
        return PoiNew.newBuilder()
                .setId(id)
                .setNode(node)
                .setBts(bts)
                .setLabelgroupnodebts(labelgroupnodebts)
                .setConfidentnodebts(confidentnodebts)
                .build();
    }

    public static ProtobufWritable<PoiNew> wrap(PoiNew obj) {
        ProtobufWritable<PoiNew> wrapper = ProtobufWritable.newInstance(
                PoiNew.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<PoiNew> createAndWrap(int id, long node,
            long bts, int labelgroupnodebts, int confidentnodebts) {
        return wrap(create(id, node, bts, labelgroupnodebts, confidentnodebts));
    }
}
