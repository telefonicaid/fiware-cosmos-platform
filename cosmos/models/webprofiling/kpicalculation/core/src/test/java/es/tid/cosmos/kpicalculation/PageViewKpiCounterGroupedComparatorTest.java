package es.tid.cosmos.kpicalculation;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 *
 * @author dmicol
 */
public class PageViewKpiCounterGroupedComparatorTest {
    private PageViewKpiCounterGroupedComparator instance;
    
    @Before
    public void setUp() {
        this.instance = new PageViewKpiCounterGroupedComparator();
    }
    
    @Test
    public void testCompare() {
        CompositeKey k1 = new BinaryKey("a", "c");
        CompositeKey k2 = new BinaryKey("b", "c");
        CompositeKey k3 = new BinaryKey("a", "c");
        assertEquals(-1, this.instance.compare(k1, k2));
        assertEquals(1, this.instance.compare(k2, k1));
        assertEquals(0, this.instance.compare(k1, k3));
    }
}
