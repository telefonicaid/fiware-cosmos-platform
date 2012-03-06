package es.tid.bdp.base.mapreduce;

import es.tid.bdp.base.mapreduce.TernaryKey;
import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.base.mapreduce.CompositeKey;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 * Test case for CompositeKey
 *
 * @author dmicol
 */
public class CompositeKeyTest {
    @Test
    public void testEqualityAndHashCode() {
        CompositeKey b1 = new BinaryKey("keya", "1"),
                     b2 = new BinaryKey("keya", "1"),
                     b3 = new BinaryKey("1", "keya");
        assertFalse(b1.equals(new Object()));
        assertTrue(b1.equals(b2));
        assertEquals(b1.hashCode(), b2.hashCode());
        assertFalse(b1.equals(b3));
        assertEquals(b1.hashCode(), b3.hashCode());

        CompositeKey t1 = new TernaryKey("keya", "1", "b"),
                     t2 = new TernaryKey("keya", "1", "b"),
                     t3 = new TernaryKey("keya", "b", "1");
        assertFalse(t1.equals(new Object()));
        assertTrue(t1.equals(t2));
        assertEquals(t1.hashCode(), t2.hashCode());
        assertFalse(t1.equals(t3));
        assertEquals(t1.hashCode(), t3.hashCode());

        assertFalse(b1.equals(t1));
        assertFalse(t1.equals(b1));
    }

    @Test
    public void shouldBeOrderedByPrimaryKey() {
        CompositeKey a = new BinaryKey("keya", "3");
        CompositeKey b = new BinaryKey("keyb", "1");

        assertEquals(-1, a.compareTo(b));
        assertEquals( 1, b.compareTo(a));
        assertEquals( 0, a.compareTo(a));
    }

    @Test
    public void shouldBeOrderedBySecondaryKey() {
        CompositeKey a = new BinaryKey("keya", "3");
        CompositeKey a2 = new BinaryKey("keya", "2");
        assertEquals(1, a.compareTo(a2));
    }
}
