package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.profile.generated.data.ProfileProtocol;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserNavigation;

/**
 * Utils for UserNavigation class.
 *
 * Extracts vititor and fullUrl fields from a log lines.
 *
 * @author sortega
 */
public final class UserNavigationUtil {
    private static final String DELIMITER = "\t";

    private static final int VISITOR_ID_INDEX = 0;
    private static final int URL_INDEX = 2;
    private static final int YEAR_INDEX = 8;
    private static final int MONTH_INDEX = 7;
    private static final int DAY_INDEX = 6;

    private UserNavigationUtil() {
    }

    public static UserNavigation create(String visitorId, String fullUrl,
            String date) {
        return UserNavigation.newBuilder().setUserId(visitorId).setUrl(fullUrl).
                setDate(date).build();
    }

    public static ProtobufWritable<UserNavigation> createAndWrap(
            String visitorId, String fullUrl, String date) {
        ProtobufWritable<ProfileProtocol.UserNavigation> wrapper =
                ProtobufWritable.newInstance(UserNavigation.class);
        wrapper.set(create(visitorId, fullUrl, date));
        return wrapper;
    }

    public static UserNavigation parse(String line) {
        String[] fields = line.split(DELIMITER);
        return create(fields[VISITOR_ID_INDEX], fields[URL_INDEX],
                      String.format("%s-%s-%s", fields[YEAR_INDEX],
                                    fields[MONTH_INDEX], fields[DAY_INDEX]));
    }
}
