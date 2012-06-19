package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsDayUtil {
    public static NodeBtsDay create(long userId, long bts,
                                    int workday, int count) {
        return NodeBtsDay.newBuilder()
                .setUserId(userId)
                .setBts(bts)
                .setWorkday(workday)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<NodeBtsDay> wrap(NodeBtsDay obj) {
        ProtobufWritable<NodeBtsDay> wrapper =
                ProtobufWritable.newInstance(NodeBtsDay.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<NodeBtsDay> createAndWrap(long userId,
            long bts, int workday, int count) {
        return wrap(create(userId, bts, workday, count));
    }
    
    public static String toString(NodeBtsDay obj, String separator) {
        return (obj.getUserId() + separator + obj.getBts() + separator +
                obj.getWorkday() + separator + obj.getCount());
    }
}