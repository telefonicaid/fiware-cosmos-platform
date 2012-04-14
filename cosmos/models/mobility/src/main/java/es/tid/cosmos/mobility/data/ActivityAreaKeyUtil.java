package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;

/**
 *
 * @author dmicol
 */
public abstract class ActivityAreaKeyUtil implements ProtobufUtil {
    public static ActivityAreaKey create(long userId, int month,
                                         boolean isWorkDay) {
        ActivityAreaKey.Builder activityAreaKey = ActivityAreaKey.newBuilder()
            .setUserId(userId)
            .setMonth(month)
            .setIsWorkDay(isWorkDay);
        return activityAreaKey.build();
    }

    public static ProtobufWritable<ActivityAreaKey> wrap(ActivityAreaKey obj) {
        ProtobufWritable<ActivityAreaKey> wrapper =
                ProtobufWritable.newInstance(ActivityAreaKey.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ActivityAreaKey> createAndWrap(long userId,
            int month, boolean isWorkDay) {
        return wrap(create(userId, month, isWorkDay));
    }
}
