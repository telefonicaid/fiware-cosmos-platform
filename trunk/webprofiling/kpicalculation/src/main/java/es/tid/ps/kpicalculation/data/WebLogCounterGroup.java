package es.tid.ps.kpicalculation.data;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Collection;

import org.apache.hadoop.io.Text;

/**
 * Class representing a WebLog in the webprofiling module used to calculate
 * simple kpis depending on a set of fields.
 * 
 * mainFields collection contains the fields that will define the kpi that will
 * be calculated
 * 
 * secondaryFields collection contains the fields that will define the field to
 * group by, when calculating the kpi
 * 
 * When write and read methods of WritableComparable interface are called during
 * map & reduce process, they will use the values of the mainFields and the
 * secondaryFields to emit and get the weblog object.
 * 
 * @author javierb
 */
public class WebLogCounterGroup extends WebLogCounter {
    private String secondaryFields;

    public WebLogCounterGroup() {
    }

    public WebLogCounterGroup(Collection<String> fields, String secondaryFields) {
        super(fields);
        this.secondaryFields = secondaryFields;
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        this.mainKey = in.readUTF();
        this.secondaryKey = in.readUTF();
    }

    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.mainKey);
        out.writeUTF(this.secondaryKey);
    }

    @Override
    public int compareTo(WebLog pv) {
        int result = this.mainKey.compareTo(pv.mainKey);
        if (result != 0) {
            return result;
        }
        return this.secondaryKey.compareTo(pv.secondaryKey);
    }

    @Override
    public int hashCode() {
        return this.mainKey.hashCode() * HASH_CONST
                + this.secondaryKey.hashCode();
    }

    @Override
    public String toString() {
        return this.mainKey + "\t" + this.secondaryKey;
    }

    @Override
    public void set(String line) throws IllegalArgumentException,
            SecurityException, IllegalAccessException, NoSuchFieldException {
        super.set(line);
        initSecondaryKey();
    }

    @Override
    public void set(Text txt) throws IllegalArgumentException,
            SecurityException, IllegalAccessException, NoSuchFieldException {
        super.set(txt);
        initSecondaryKey();
    }

    protected void initSecondaryKey() throws IllegalArgumentException,
            SecurityException, IllegalAccessException, NoSuchFieldException {
        this.secondaryKey = (String) this.getClass()
                .getField(this.secondaryFields).get(this);
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object o) {
        if (o instanceof WebLogCounterGroup) {
            WebLogCounterGroup pv = (WebLogCounterGroup) o;
            return this.mainKey.equals(pv.mainKey)
                    && this.secondaryKey.equals(pv.secondaryKey);
        }
        return false;
    }
}