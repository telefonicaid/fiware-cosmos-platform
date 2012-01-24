package es.tid.ps.profile.categoryextraction;

/**
 * Extracts vititor and fullUrl fields from a log lines.
 *
 * @author sortega
 */
public class UserNavigation {
    private static final String DELIMITER = "\t";
    
    private String visitorId;
    private String fullUrl;

    public UserNavigation() {
    }

    public String getVisitorId() {
        return this.visitorId;
    }
    
    public void setVisitorId(String visitorId) {
        this.visitorId = visitorId;
    }
    
    public String getFullUrl() {
        return this.fullUrl;
    }
    
    public void setFullUrl(String fullUrl) {
        this.fullUrl = fullUrl;
    }
    
    public void parse(String line) {
        String[] fields = line.split(DELIMITER);
        this.visitorId = fields[0];
        this.fullUrl = fields[2];
    }
}
