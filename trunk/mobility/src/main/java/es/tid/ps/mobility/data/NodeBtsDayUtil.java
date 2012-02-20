package es.tid.ps.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.ps.mobility.data.MxProtocol.NodeBtsDay;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsDayUtil {
    public static NodeBtsDay create(long node, int bts, int workday, int count) {
        return NodeBtsDay.newBuilder()
                .setNode(node)
                .setBts(bts)
                .setWorkday(workday)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<NodeBtsDay> createAndWrap(long node, int bts,
            int workday, int count) {
        ProtobufWritable<NodeBtsDay> wrapper =
                ProtobufWritable.newInstance(NodeBtsDay.class);
        wrapper.set(create(node, bts, workday, count));
        return wrapper;
    }
}