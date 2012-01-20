package es.tid.ps.dynamicprofile.categoryextraction;

/* 
 * Representation of a line that contains web log information.
 * 
 * @author sortega
 */
public class WebLog {
    private static final String DELIMITER ="\t";
    
    public String visitorId;
    public String fullUrl;

    public void set(String line) throws IllegalArgumentException,
            SecurityException, IllegalAccessException, NoSuchFieldException {
        String[] fields = line.split(DELIMITER);

        this.visitorId = fields[0];
        this.fullUrl = fields[2];
    }
}
