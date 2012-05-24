package es.tid.cosmos.base.mapreduce;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import org.apache.hadoop.io.WritableComparable;

/**
 * The comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public class CompositeKey implements WritableComparable<CompositeKey> {
    private static final int DEFAULT_KEY_SIZE = 2;
    
    private String[] keys;
    
    public CompositeKey() {
        this(DEFAULT_KEY_SIZE);
    }
    
    public CompositeKey(int capacity) {
        this.keys = new String[capacity];
    }
    
    public void set(int index, String key) {
        this.keys[index] = key;
    }

    public String get(int index) {
        return this.keys[index];
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.hadoop.io.Writable#write(java.io.DataOutput)
     */
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeInt(this.keys.length);
        for (String key : this.keys) {
            out.writeUTF(key);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.hadoop.io.Writable#readFields(java.io.DataInput)
     */
    @Override
    public void readFields(DataInput in) throws IOException {
        int capacity = in.readInt();
        this.keys = new String[capacity];
        for (int i = 0; i < capacity; i++) {
            this.keys[i] = in.readUTF();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        int hash = 1;
        for (String key : keys) {
            hash *= key.hashCode();
        }
        return hash;
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
        if (tp.keys.length != this.keys.length) {
            return false;
        }
        for (int i = 0; i < this.keys.length; i++) {
            if (!tp.keys[i].equals(this.keys[i])) {
                return false;
            }
        }
        return true;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        String str = "";
        for (String key : this.keys) {
            if (!str.isEmpty()) {
                str += "\t";
            }
            str += key;
        }
        return str;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Comparable#compareTo(java.lang.Object)
     */
    @Override
    public int compareTo(CompositeKey tp) {
        int i = 0;
        while (i < tp.keys.length && i < this.keys.length) {
            int cmp = this.keys[i].compareTo(tp.keys[i]);
            if (cmp != 0) {
                return cmp;
            }
            i++;
        }
        if (this.keys.length < tp.keys.length) {
            return -1;
        } else if (this.keys.length > tp.keys.length) {
            return 1;
        } else {
            return 0;
        }
    }
}
