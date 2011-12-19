package es.tid.ps.kpicalculation.data;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Collection;
import java.util.Iterator;

import org.apache.hadoop.io.Text;

/**
 * Class representing a WebLog in the webprofiling module used to calculate
 * simple kpis depending on a set of fields.
 * 
 * mainFields collection contains the fields that will define the kpi that will
 * be calculated
 * 
 * When write and read methods of WritableComparable interface are called during
 * map & reduce process, they will use only the values of those mainFields to
 * emit and get the weblog object.
 * 
 * @author javierb
 */
public class WebLogCounter extends WebLog {
    protected static int HASH_CONST = 163;

    private Iterator<String> iterator;
    protected Collection<String> mainFields;

    public WebLogCounter() {
    }

    public WebLogCounter(Collection<String> fields) {
        this.mainFields = fields;
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        this.mainKey = in.readUTF();
    }

    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.mainKey);
    }

    @Override
    public int compareTo(WebLog pv) {
        return this.mainKey.compareTo(pv.mainKey);
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        return this.mainKey.hashCode() * HASH_CONST;
    }

    @Override
    public String toString() {
        return this.mainKey;
    }

    @Override
    public void set(String line) throws IllegalArgumentException,
            SecurityException, IllegalAccessException, NoSuchFieldException {
        // TODO Auto-generated method stub
        super.set(line);
        if (this.mainFields != null) {
            this.initMainKey(this.mainFields);
        }
    }

    @Override
    public void set(Text txt) throws IllegalArgumentException,
            SecurityException, IllegalAccessException, NoSuchFieldException {
        // TODO Auto-generated method stub
        super.set(txt);
        if (this.mainFields != null) {
            this.initMainKey(this.mainFields);
        }
    }

    protected void initMainKey(Collection<String> mainFields)
            throws IllegalArgumentException, SecurityException,
            IllegalAccessException, NoSuchFieldException {
        this.mainKey = "";
        this.iterator = mainFields.iterator();

        while (this.iterator.hasNext()) {
            this.mainKey += (String) this.getClass()
                    .getField(this.iterator.next()).get(this);
            if (this.iterator.hasNext()) {
                this.mainKey += "\t";
            }
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object o) {
        if (o instanceof WebLogCounter) {
            WebLogCounter pv = (WebLogCounter) o;
            return this.mainKey.equals(pv.mainKey);
        }
        return false;
    }
}
