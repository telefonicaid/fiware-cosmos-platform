package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsUtil {
    public static final String DELIMITER = "|";
    
    public static NodeBts create(long userId, long bts,
                                 int weekday, int range) {
        return NodeBts.newBuilder()
                .setUserId(userId)
                .setBts(bts)
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
            long bts, int weekday, int range) {
        return wrap(create(userId, bts, weekday, range));
    }
    
    public static String toString(NodeBts obj) {
        return obj.getUserId() + DELIMITER + obj.getBts() + DELIMITER
                + obj.getWeekday() + DELIMITER + obj.getRange();
    }
}

