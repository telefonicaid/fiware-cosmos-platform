package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;

/**
 *
 * @author dmicol
 */
public final class ItinMovementUtil {

    private ItinMovementUtil() {}

    public static ItinMovement create(ItinTime source, ItinTime target) {
        return ItinMovement.newBuilder()
                .setSource(source)
                .setTarget(target)
                .build();
    }

    public static ProtobufWritable<ItinMovement> wrap(ItinMovement obj) {
        ProtobufWritable<ItinMovement> wrapper = ProtobufWritable.newInstance(
                ItinMovement.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<ItinMovement> createAndWrap(ItinTime source,
            ItinTime target) {
        return wrap(create(source, target));
    }
}
