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
        super();
    }

    public WebLogCounter(Collection<String> fields) {
        super();
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
        if (mainFields != null)
            this.initMainKey(this.mainFields);
    }

    @Override
    public void set(Text txt) throws IllegalArgumentException,
            SecurityException, IllegalAccessException, NoSuchFieldException {
        // TODO Auto-generated method stub
        super.set(txt);
        if (mainFields != null)
            this.initMainKey(this.mainFields);
    }

    protected void initMainKey(Collection<String> mainFields)
            throws IllegalArgumentException, SecurityException,
            IllegalAccessException, NoSuchFieldException {
        mainKey = "";
        this.iterator = mainFields.iterator();

        while (iterator.hasNext()) {
            mainKey += (String) this.getClass().getField(iterator.next())
                    .get(this);
            if (iterator.hasNext()) {
                mainKey += "\t";
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
