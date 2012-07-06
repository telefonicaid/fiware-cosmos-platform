package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;

/**
 *
 * @author dmicol
 */
public final class PoiNewUtil {

    private PoiNewUtil() {}

    public static PoiNew create(long id, long node, long bts,
            int labelGroupNodeBts, int confidentNodeBts) {
        return PoiNew.newBuilder()
                .setId(id)
                .setNode(node)
                .setBts(bts)
                .setLabelgroupnodebts(labelGroupNodeBts)
                .setConfidentnodebts(confidentNodeBts)
                .build();
    }

    public static ProtobufWritable<PoiNew> wrap(PoiNew obj) {
        ProtobufWritable<PoiNew> wrapper = ProtobufWritable.newInstance(
                PoiNew.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<PoiNew> createAndWrap(long id, long node,
            long bts, int labelGroupNodeBts, int confidentNodeBts) {
        return wrap(create(id, node, bts, labelGroupNodeBts, confidentNodeBts));
    }
}
