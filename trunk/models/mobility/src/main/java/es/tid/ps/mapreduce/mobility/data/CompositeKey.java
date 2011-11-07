package es.tid.ps.mapreduce.mobility.data;

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
 * @author rgc
 */
public class CompositeKey implements WritableComparable<CompositeKey> {
    private String userIdKey;
    private Integer fileIdKey;

    public CompositeKey() {
        init(new String(), -1);
    }

    public CompositeKey(String userIdKey, Integer fileIdKey) {
        init(userIdKey, fileIdKey);
    }

    public void init(String userIdKey, Integer fileIdKey) {
        this.userIdKey = userIdKey;
        this.fileIdKey = fileIdKey;
    }

    /**
     * Get the userIdKey
     * 
     * @return the userIdKey
     */
    public String getUserIdKey() {
        return this.userIdKey;
    }

    /**
     * Get the fileIdKey
     * 
     * @return the fileIdKey
     */
    public int getFileIdKey() {
        return this.fileIdKey;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.Writable#write(java.io.DataOutput)
     */
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.userIdKey);
        out.writeInt(this.fileIdKey);
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.Writable#readFields(java.io.DataInput)
     */
    @Override
    public void readFields(DataInput in) throws IOException {
        this.userIdKey = in.readUTF();
        this.fileIdKey = in.readInt();
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        return this.userIdKey.hashCode() * 163 + this.fileIdKey;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object o) {
        if (o instanceof CompositeKey) {
            CompositeKey tp = (CompositeKey) o;
            return this.userIdKey.equals(tp.userIdKey)
                    && this.fileIdKey.equals(tp.fileIdKey);
        }
        return false;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return this.userIdKey + "\t" + this.fileIdKey;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Comparable#compareTo(java.lang.Object)
     */
    @Override
    public int compareTo(CompositeKey tp) {
        int cmp = this.userIdKey.compareTo(tp.userIdKey);
        if (cmp != 0) {
            return cmp;
        }
        return this.fileIdKey.compareTo(tp.fileIdKey);
    }
}
