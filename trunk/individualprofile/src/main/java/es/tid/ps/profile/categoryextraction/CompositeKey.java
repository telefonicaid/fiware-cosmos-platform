package es.tid.ps.profile.categoryextraction;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import org.apache.hadoop.io.WritableComparable;

/**
 * The key type for this Map Reduce application.
 *
 * This particular key has two parts, the String users identifier and file type
 * identifier.
 *
 * @author dmicol, rgc
 */
public class CompositeKey implements WritableComparable<CompositeKey> {
    private String primaryKey;
    private String secondaryKey;

    public CompositeKey() {
        this.primaryKey = "";
        this.secondaryKey = "";
    }

    public CompositeKey(String primaryKey, String secondaryKey) {
        this.primaryKey = primaryKey;
        this.secondaryKey = secondaryKey;
    }

    /**
     * Get the userIdKey
     *
     * @return the userIdKey
     */
    public String getPrimaryKey() {
        return this.primaryKey;
    }

    /**
     * Get the fileIdKey
     *
     * @return the fileIdKey
     */
    public String getSecondaryKey() {
        return this.secondaryKey;
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.hadoop.io.Writable#write(java.io.DataOutput)
     */
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.primaryKey);
        out.writeUTF(this.secondaryKey);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.hadoop.io.Writable#readFields(java.io.DataInput)
     */
    @Override
    public void readFields(DataInput in) throws IOException {
        this.primaryKey = in.readUTF();
        this.secondaryKey = in.readUTF();
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        return this.primaryKey.hashCode() * this.secondaryKey.hashCode();
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof CompositeKey)) {
            return false;
        }
        CompositeKey tp = (CompositeKey) o;
        return this.primaryKey.equals(tp.primaryKey)
                && this.secondaryKey.equals(tp.secondaryKey);
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return this.primaryKey + "\t" + this.secondaryKey;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Comparable#compareTo(java.lang.Object)
     */
    @Override
    public int compareTo(CompositeKey tp) {
        int cmp = this.primaryKey.compareTo(tp.primaryKey);
        if (cmp != 0) {
            return cmp;
        }
        return this.secondaryKey.compareTo(tp.secondaryKey);
    }
}
