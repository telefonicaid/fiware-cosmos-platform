package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;

/**
 *
 * @author dmicol
 */
public final class ItinPercMoveUtil {

    private ItinPercMoveUtil() {}

    public static ItinPercMove create(int group, int range, double percMoves) {
        return ItinPercMove.newBuilder()
                .setGroup(group)
                .setRange(range)
                .setPercMoves(percMoves)
                .build();
    }

    public static ProtobufWritable<ItinPercMove> wrap(ItinPercMove obj) {
        ProtobufWritable<ItinPercMove> wrapper = ProtobufWritable.newInstance(
                ItinPercMove.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ItinPercMove> createAndWrap(int group,
            int range, double percMoves) {
        return wrap(create(group, range, percMoves));
    }
}
