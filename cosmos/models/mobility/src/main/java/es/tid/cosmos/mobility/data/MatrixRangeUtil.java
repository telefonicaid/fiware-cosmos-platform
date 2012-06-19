package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;

/**
 *
 * @author dmicol
 */
public final class MatrixRangeUtil {

    private MatrixRangeUtil() {}

    public static MatrixRange create(long node, long poiSrc, long poiTgt,
                                     int group, int range) {
        return MatrixRange.newBuilder()
                .setNode(node)
                .setPoiSrc(poiSrc)
                .setPoiTgt(poiTgt)
                .setGroup(group)
                .setRange(range)
                .build();
    }

    public static ProtobufWritable<MatrixRange> wrap(MatrixRange obj) {
        ProtobufWritable<MatrixRange> wrapper = ProtobufWritable.newInstance(
                MatrixRange.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<MatrixRange> createAndWrap(long node,
            long poiSrc, long poiTgt, int group, int range) {
        return wrap(create(node, poiSrc, poiTgt, group, range));
    }
}
