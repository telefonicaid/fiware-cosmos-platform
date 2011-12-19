package es.tid.ps.kpicalculation;

import java.io.IOException;
import java.util.Collection;

import org.apache.hadoop.io.DataInputBuffer;
import org.apache.hadoop.io.RawComparator;

import es.tid.ps.kpicalculation.data.WebLog;
import es.tid.ps.kpicalculation.data.WebLogCounter;

/**
 * Class used in the compare and sort phases of process for simple aggregation
 * operations. This class is used to compare items of type WebLogCounter.
 * 
 * Note: Ideally this class should be used in a general way to use subclasses of
 * WebLog through casting operations. At this moment, and due to the flow of
 * hadoop map & reduce process, it is not possible to set a dynamic type to the
 * comparation objects so we have used to different comparator classes
 * 
 * @author javierb
 */
public class PageViewKpiCounterComparator implements RawComparator<WebLog> {
    private final DataInputBuffer buffer;

    public PageViewKpiCounterComparator() {
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
            WebLogCounter o1 = new WebLogCounter();
            WebLogCounter o2 = new WebLogCounter();
            this.buffer.reset(b1, s1, l1);
            o1.readFields(buffer);
            this.buffer.reset(b2, s2, l2);
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
    public int compare(WebLog o1, WebLog o2) {
        return o1.compareTo(o2);
    }
}