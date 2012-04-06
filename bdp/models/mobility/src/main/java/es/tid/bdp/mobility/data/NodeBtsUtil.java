package es.tid.bdp.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.mobility.data.MobProtocol.NodeBts;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsUtil implements ProtobufUtil {
    public static NodeBts create(long userId, int placeId,
                                 int weekday, int range) {
        return NodeBts.newBuilder()
                .setUserId(userId)
                .setPlaceId(placeId)
                .setWeekday(weekday)
                .setRange(range)
                .build();
    }

    public static ProtobufWritable<NodeBts> createAndWrap(long userId,
            int placeId, int weekday, int range) {
        ProtobufWritable<NodeBts> wrapper =
                ProtobufWritable.newInstance(NodeBts.class);
        wrapper.set(create(userId, placeId, weekday, range));
        return wrapper;
    }
}

