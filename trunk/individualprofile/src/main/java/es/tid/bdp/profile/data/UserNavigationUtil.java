package es.tid.bdp.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.profile.data.ProfileProtocol.UserNavigation;

/**
 * Construction utils for UserNavigation class.
 *
 * Extracts vititor and fullUrl fields from a log lines.
 *
 * @author sortega
 */
public abstract class UserNavigationUtil {
    private static final String DELIMITER = "\t";
    
    private UserNavigationUtil() {
    }

    public static UserNavigation create(String visitorId, String fullUrl,
            String date) {
        return UserNavigation.newBuilder().setUserId(visitorId).setUrl(fullUrl).
                setDate(date).build();
    }

    public static ProtobufWritable createAndWrap(String visitorId,
            String fullUrl, String date) {
        ProtobufWritable<ProfileProtocol.UserNavigation> wrapper =
                ProtobufWritable.newInstance(UserNavigation.class);
        wrapper.set(create(visitorId, fullUrl, date));
        return wrapper;
    }

    public static UserNavigation parse(String line) {
        String[] fields = line.split(DELIMITER);
        return create(fields[0], fields[2],
                String.format("%s-%s-%s", fields[8], fields[7], fields[6]));
    }
}
