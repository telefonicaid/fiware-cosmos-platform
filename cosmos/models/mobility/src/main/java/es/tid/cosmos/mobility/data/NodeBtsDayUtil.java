package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 *
 * @author sortega
 */
public abstract class NodeBtsDayUtil {
    public static final String DELIMITER = "|";
    
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
    
    public static NodeBtsDay parse(String line) {
        String[] values = line.split(DELIMITER);
        return create(Long.parseLong(values[0]), Integer.parseInt(values[1]),
                      Integer.parseInt(values[2]), Integer.parseInt(values[3]));
    }
    
    public static String toString(NodeBtsDay obj) {
        return (obj.getUserId() + DELIMITER + obj.getBts() + DELIMITER +
                obj.getWorkday() + DELIMITER + obj.getCount());
    }
}