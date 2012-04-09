package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.Bts;

/**
 *
 * @author sortega
 */
public abstract class BtsUtil implements ProtobufUtil {

    public static Bts create(long placeId, double posx, double posy,
            double area, Iterable<Long> adjBts) {
        Bts.Builder bts = Bts.newBuilder()
                .setPlaceId(placeId)
                .setPosx(posx)
                .setPosy(posy)
                .setArea(area)
                .addAllAdjBts(adjBts);
        return bts.build();
    }

    public static ProtobufWritable<Bts> createAndWrap(long placeId, double posx,
            double posy, double area, Iterable<Long> adjBts) {
        ProtobufWritable<Bts> wrapper =
                ProtobufWritable.newInstance(Bts.class);
        wrapper.set(create(placeId, posx, posy, area, adjBts));
        return wrapper;
    }
}