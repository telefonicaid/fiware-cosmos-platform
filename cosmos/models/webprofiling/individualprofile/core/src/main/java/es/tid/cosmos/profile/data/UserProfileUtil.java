package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 * Utils for UserProfile class.
 *
 * @author dmicol
 */
public final class UserProfileUtil {
    private static final String DELIMITER = "\t";

    private UserProfileUtil() {
    }

    public static UserProfile create(String userId, String date,
                                     Iterable<CategoryCount> counts) {
        return UserProfile.newBuilder()
                .setUserId(userId)
                .setDate(date)
                .addAllCounts(counts)
                .build();
    }

    public static ProtobufWritable<UserProfile> createAndWrap(String userId,
            String date, Iterable<CategoryCount> counts) {
        ProtobufWritable<UserProfile> wrapper = ProtobufWritable.newInstance(
                UserProfile.class);
        wrapper.set(create(userId, date, counts));
        return wrapper;
    }
    
    public static String toString(UserProfile obj) {
        String str = obj.getUserId() + DELIMITER + obj.getDate();
        for (CategoryCount count : obj.getCountsList()) {
            str += DELIMITER + CategoryCountUtil.toString(count);
        }
        return str;
    }
}
