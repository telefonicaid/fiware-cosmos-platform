package es.tid.bdp.kpicalculation;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.base.mapreduce.CompositeKey;
import es.tid.bdp.base.mapreduce.SingleKey;

/**
 *
 * @author dmicol
 */
public class PageViewKpiCounterComparatorTest {
    private PageViewKpiCounterComparator instance;
    
    @Before
    public void setUp() {
        this.instance = new PageViewKpiCounterComparator();
    }
    
    @Test
    public void testCompare() {
        CompositeKey k1 = new SingleKey("a");
        CompositeKey k2 = new SingleKey("b");
        CompositeKey k3 = new SingleKey("a");
        assertEquals(-1, this.instance.compare(k1, k2));
        assertEquals(1, this.instance.compare(k2, k1));
        assertEquals(0, this.instance.compare(k1, k3));
    }
}
