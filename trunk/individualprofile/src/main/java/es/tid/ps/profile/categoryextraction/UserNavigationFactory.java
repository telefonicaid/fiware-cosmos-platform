package es.tid.ps.profile.categoryextraction;

import java.util.Calendar;

/**
 * Extracts visitor and fullUrl fields from a log lines.
 *
 * @author sortega, dmicol
 */
public abstract class UserNavigationFactory {
    private static final String DELIMITER = "\t";

    public static void set(UserNavigation nav, String visitorId, String fullUrl,
                           Calendar date) {
        nav.setVisitorId(visitorId);
        nav.setFullUrl(fullUrl);
        //nav.setDate(date.toString());
    }

    public static void set(UserNavigation nav, String line) {
        String[] fields = line.split(DELIMITER);
        nav.setVisitorId(fields[0]);
        nav.setFullUrl(fields[2]);
        Calendar date = Calendar.getInstance();
        date.set(Integer.parseInt(fields[8]),
                Integer.parseInt(fields[7]),
                Integer.parseInt(fields[6]));
        //nav.setDate(date.toString());
    }
}
