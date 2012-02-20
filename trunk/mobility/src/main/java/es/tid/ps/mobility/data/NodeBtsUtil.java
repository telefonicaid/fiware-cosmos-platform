package es.tid.ps.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.ps.mobility.data.MxProtocol.NodeBts;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsUtil {
    public static NodeBts create(long phone, int bts, int wday, int range) {
        return NodeBts.newBuilder()
                .setPhone(phone)
                .setBts(bts)
                .setWday(wday)
                .setRange(range)
                .build();
    }

    public static ProtobufWritable<NodeBts> createAndWrap(long phone, int bts,
            int wday, int range) {
        ProtobufWritable<NodeBts> wrapper =
                ProtobufWritable.newInstance(NodeBts.class);
        wrapper.set(create(phone, bts, wday, range));
        return wrapper;
    }
}

