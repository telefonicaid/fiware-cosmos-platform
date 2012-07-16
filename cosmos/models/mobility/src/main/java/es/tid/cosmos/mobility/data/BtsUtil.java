package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 *
 * @author dmicol, sortega
 */
public final class BtsUtil {

    private BtsUtil() {}

    public static Bts create(long placeId, long comms, double posx, double posy,
            double area, Iterable<Long> adjBts) {
        Bts.Builder bts = Bts.newBuilder()
                .setPlaceId(placeId)
                .setComms(comms)
                .setPosx(posx)
                .setPosy(posy)
                .setArea(area)
                .addAllAdjBts(adjBts);
        return bts.build();
    }

    public static ProtobufWritable<Bts> wrap(Bts obj) {
        ProtobufWritable<Bts> wrapper = ProtobufWritable.newInstance(Bts.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Bts> createAndWrap(long placeId, long comms,
            double posx, double posy, double area, Iterable<Long> adjBts) {
        return wrap(create(placeId, comms, posx, posy, area, adjBts));
    }
}