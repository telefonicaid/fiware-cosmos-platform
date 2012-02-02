package es.tid.ps.profile.categoryextraction;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Calendar;
import org.apache.hadoop.io.Writable;

/**
 * Extracts vititor and fullUrl fields from a log lines.
 *
 * @author sortega
 */
public class UserNavigation implements Writable {
    private static final String DELIMITER = "\t";
    
    private String visitorId;
    private String fullUrl;
    private Calendar date;

    public UserNavigation() {
    }

    public UserNavigation(String visitorId, String fullUrl, Calendar date) {
        this.visitorId = visitorId;
        this.fullUrl = fullUrl;
        this.date = Calendar.getInstance();
        this.date.set(Calendar.YEAR, date.get(Calendar.YEAR));
        this.date.set(Calendar.MONTH, date.get(Calendar.MONTH));
        this.date.set(Calendar.DATE, date.get(Calendar.DATE));
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
    
    public Calendar getDate() {
        return this.date;
    }
    
    public void setDate(Calendar date) {
        this.date = date;
    }
    
    public void parse(String line) {
        String[] fields = line.split(DELIMITER);
        this.visitorId = fields[0];
        this.fullUrl = fields[2];
        this.date = Calendar.getInstance();
        this.date.set(Integer.parseInt(fields[8]),
                      Integer.parseInt(fields[7]),
                      Integer.parseInt(fields[6]));
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        this.visitorId = in.readUTF();
        this.fullUrl = in.readUTF();
        this.date = Calendar.getInstance();
        this.date.set(Calendar.YEAR, in.readInt());
        this.date.set(Calendar.MONTH, in.readInt());
        this.date.set(Calendar.DATE, in.readInt());
    }
    
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.visitorId);
        out.writeUTF(this.fullUrl);
        out.writeInt(this.date.get(Calendar.YEAR));
        out.writeInt(this.date.get(Calendar.MONTH));
        out.writeInt(this.date.get(Calendar.DATE));
    }
}
