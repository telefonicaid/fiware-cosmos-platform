package es.tid.ps.profile.categoryextraction;

/**
 * Extracts vititor and fullUrl fields from a log lines.
 *
 * @author sortega
 */
public class WebLog {
    private static final String DELIMITER ="\t";

    public String visitorId;
    public String fullUrl;

    public void set(String line) {
        String[] fields = line.split(DELIMITER);

        this.visitorId = fields[0];
        this.fullUrl = fields[2];
    }
}
