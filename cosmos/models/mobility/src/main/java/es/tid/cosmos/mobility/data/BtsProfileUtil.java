package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;

/**
 *
 * @author dmicol
 */
public abstract class BtsProfileUtil {
    public static final String DELIMITER = "|";
    
    public static BtsProfile create(long placeId, int profile, int wday,
                                    int hour) {
        return BtsProfile.newBuilder()
                .setPlaceId(placeId)
                .setProfile(profile)
                .setWday(wday)
                .setHour(hour)
                .build();
    }

    public static ProtobufWritable<BtsProfile> wrap(BtsProfile obj) {
        ProtobufWritable<BtsProfile> wrapper =
                ProtobufWritable.newInstance(BtsProfile.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<BtsProfile> createAndWrap(long placeId,
            int profile, int wday, int hour) {
        return wrap(create(placeId, profile, wday, hour));
    }
}
