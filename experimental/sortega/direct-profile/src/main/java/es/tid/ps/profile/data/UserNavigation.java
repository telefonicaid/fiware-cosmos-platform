package es.tid.ps.profile.data;

/**
 *
 * @author sortega
 */
public class UserNavigation {
    private static final String DELIMITER = "\t";
    private String visitorId;
    private String url;
    private String date;

    public String getDate() {
        return date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getVisitorId() {
        return visitorId;
    }

    public void setVisitorId(String visitorId) {
        this.visitorId = visitorId;
    }

    public static UserNavigation parse(String line) {
        String[] fields = line.split(DELIMITER);
        UserNavigation instance = new UserNavigation();
        instance.setVisitorId(fields[0]);
        instance.setUrl(fields[2]);
        instance.setDate(String.format("%s-%s-%s", fields[8], fields[7], fields[6]));
        return instance;
    }
}
