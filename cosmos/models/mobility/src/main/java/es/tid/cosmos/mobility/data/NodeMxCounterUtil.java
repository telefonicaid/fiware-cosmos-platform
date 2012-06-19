package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public final class NodeMxCounterUtil {

    private NodeMxCounterUtil() {}

    public static NodeMxCounter create(Iterable<BtsCounter> allBts,
                                       int btsLength,
                                       int btsMaxLength) {
        return NodeMxCounter.newBuilder()
                .addAllBts(allBts)
                .setBtsLength(btsLength)
                .setBtsMaxLength(btsMaxLength)
                .build();
    }

    public static ProtobufWritable<NodeMxCounter> wrap(NodeMxCounter obj) {
        ProtobufWritable<NodeMxCounter> wrapper =
                ProtobufWritable.newInstance(NodeMxCounter.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<NodeMxCounter> createAndWrap(
            Iterable<BtsCounter> allBts, int btsLength, int btsMaxLength) {
        return wrap(create(allBts, btsLength, btsMaxLength));
    }
}
