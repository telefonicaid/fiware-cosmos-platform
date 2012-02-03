package es.tid.ps.base.mapreduce;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import org.apache.hadoop.io.WritableComparable;

/**
 * The comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public abstract class CompositeKey implements WritableComparable<CompositeKey> {
    private int capacity;
    private List<String> keys;

    public CompositeKey() {
        this.capacity = 0;
        this.keys = null;
    }

    public CompositeKey(int capacity) {
        if (capacity < 1) {
            throw new IllegalArgumentException("Invalid capacity.");
        }
        this.capacity = capacity;
        this.keys = new LinkedList<String>();
    }

    public void set(int index, String key) {
        if (this.keys.size() == this.capacity) {
            throw new IllegalStateException("Capacity exceeded.");
        }
        this.keys.add(key);
    }

    public String get(int index) {
        return this.keys.get(index);
    }

    /*
     * (non-Javadoc)
     *
     * @see org.apache.hadoop.io.Writable#write(java.io.DataOutput)
     */
    @Override
    public void write(DataOutput out) throws IOException {
        out.writeInt(this.capacity);
        for (String key : keys) {
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
        this.capacity = in.readInt();
        this.keys = new LinkedList<String>();
        for (int i = 0; i < this.capacity; i++) {
            this.keys.add(in.readUTF());
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
        if (tp.capacity != this.capacity) {
            return false;
        }
        for (int i = 0; i < this.capacity; i++) {
            if (!tp.keys.get(i).equals(this.keys.get(i))) {
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
        for (String key : keys) {
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
        while (i < tp.capacity && i < this.capacity) {
            int cmp = this.keys.get(i).compareTo(tp.keys.get(i));
            if (cmp != 0) {
                return cmp;
            }
            i++;
        }
        if (this.capacity < tp.capacity) {
            return -1;
        } else if (this.capacity > tp.capacity) {
            return 1;
        } else {
            return 0;
        }
    }
}
