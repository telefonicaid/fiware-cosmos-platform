package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsUtil implements ProtobufUtil {
    private static final String DELIMITER = "|";
    
    public static NodeBts create(long userId, int placeId,
                                 int weekday, int range) {
        return NodeBts.newBuilder()
                .setUserId(userId)
                .setPlaceId(placeId)
                .setWeekday(weekday)
                .setRange(range)
                .build();
    }

    public static ProtobufWritable<NodeBts> wrap(NodeBts obj) {
        ProtobufWritable<NodeBts> wrapper =
                ProtobufWritable.newInstance(NodeBts.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<NodeBts> createAndWrap(long userId,
            int placeId, int weekday, int range) {
        return wrap(create(placeId, placeId, weekday, range));
    }
    
    public static NodeBts parse(String line) {
        String[] values = line.split(DELIMITER);
        return create(Long.parseLong(values[0]), Integer.parseInt(values[1]),
                      Integer.parseInt(values[2]), Integer.parseInt(values[3]));
    }
}

