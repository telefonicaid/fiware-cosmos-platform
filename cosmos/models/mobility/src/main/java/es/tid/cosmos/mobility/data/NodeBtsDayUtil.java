package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsDayUtil implements ProtobufUtil {
    public static NodeBtsDay create(long userId, long placeId,
                                    int workday, int count) {
        return NodeBtsDay.newBuilder()
                .setUserId(userId)
                .setPlaceId(placeId)
                .setWorkday(workday)
                .setCount(count)
                .build();
    }

    public static ProtobufWritable<NodeBtsDay> createAndWrap(long userId,
            long placeId, int workday, int count) {
        ProtobufWritable<NodeBtsDay> wrapper =
                ProtobufWritable.newInstance(NodeBtsDay.class);
        wrapper.set(create(userId, placeId, workday, count));
        return wrapper;
    }
}