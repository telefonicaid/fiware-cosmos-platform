package es.tid.ps.mapreduce.mobility;

import java.io.IOException;

import org.apache.hadoop.io.DataInputBuffer;
import org.apache.hadoop.io.RawComparator;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;

/**
 * Class that use to compare and sort the class CompositeKey for sorting all for
 * grouping
 * 
 * @author rgc
 * 
 */
public class NaturalKeySortComparator implements RawComparator<CompositeKey> {
    private final DataInputBuffer buffer;

    public NaturalKeySortComparator() {
        this.buffer = new DataInputBuffer();
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.apache.hadoop.io.RawComparator#compare(byte[], int, int, byte[],
     * int, int)
     */
    @Override
    public int compare(byte[] b1, int s1, int l1, byte[] b2, int s2, int l2) {
        try {
            CompositeKey o1 = new CompositeKey();
            CompositeKey o2 = new CompositeKey();
            buffer.reset(b1, s1, l1);
            o1.readFields(buffer);
            buffer.reset(b2, s2, l2);
            o2.readFields(buffer);
            return compare(o1, o2);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
     */
    @Override
    public int compare(CompositeKey o1, CompositeKey o2) {
        return o1.compareTo(o2);
    }
}