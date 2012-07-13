package es.tid.cosmos.mobility.data;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol, sortega
 */
public final class NodeMxCounterUtil {

    private NodeMxCounterUtil() {}

    public static NodeMxCounter create(Iterable<BtsCounter> allBts) {
        return NodeMxCounter.newBuilder()
                .addAllBts(allBts)
                .build();
    }

    public static NodeMxCounter create(BtsCounter... allBts) {
        return NodeMxCounter.newBuilder()
                .addAllBts(asList(allBts))
                .build();
    }

    public static ProtobufWritable<NodeMxCounter> wrap(NodeMxCounter obj) {
        ProtobufWritable<NodeMxCounter> wrapper =
                ProtobufWritable.newInstance(NodeMxCounter.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<NodeMxCounter> createAndWrap(
            Iterable<BtsCounter> allBts) {
        return wrap(create(allBts));
    }
}
