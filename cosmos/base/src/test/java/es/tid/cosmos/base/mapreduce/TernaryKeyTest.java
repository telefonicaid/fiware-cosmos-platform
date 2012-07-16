package es.tid.cosmos.base.mapreduce;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 * Test case for BinaryKey
 *
 * @author dmicol
 */
public class TernaryKeyTest {
    @Test
    public void testEqualityAndHashCode() {
        CompositeKey t1 = new TernaryKey("keya", "1", "b"),
                     t2 = new TernaryKey("keya", "1", "b"),
                     t3 = new TernaryKey("keya", "b", "1");
        assertFalse(t1.equals(new Object()));
        assertTrue(t1.equals(t2));
        assertEquals(t1.hashCode(), t2.hashCode());
        assertFalse(t1.equals(t3));
        assertEquals(t1.hashCode(), t3.hashCode());
    }

    @Test
    public void testGettersAndSetters() {
        TernaryKey t1 = new TernaryKey("keya", "1", "b");
        assertEquals("keya", t1.getPrimaryKey());
        assertEquals("1", t1.getSecondaryKey());
        assertEquals("b", t1.getTertiaryKey());

        TernaryKey t2 = new TernaryKey();
        t2.setPrimaryKey("keya");
        t2.setSecondaryKey("1");
        t2.setTertiaryKey("b");

        assertTrue(t1.equals(t2));
    }

    @Test
    public void shouldBeOrderedByPrimaryKey() {
        CompositeKey a = new TernaryKey("keya", "3", "b");
        CompositeKey b = new TernaryKey("keyb", "1", "b");
        assertEquals(-1, a.compareTo(b));
        assertEquals( 1, b.compareTo(a));
        assertEquals( 0, a.compareTo(a));
    }

    @Test
    public void shouldBeOrderedBySecondaryKey() {
        CompositeKey a = new TernaryKey("keya", "3", "c");
        CompositeKey a2 = new TernaryKey("keya", "2", "c");
        assertEquals(1, a.compareTo(a2));
    }

    @Test
    public void shouldBeOrderedByTertiaryKey() {
        CompositeKey a = new TernaryKey("keya", "2", "d");
        CompositeKey a2 = new TernaryKey("keya", "2", "c");
        assertEquals(1, a.compareTo(a2));
    }

    @Test
    public void testToString() {
        CompositeKey a = new TernaryKey("keya", "2", "d");
        assertEquals("keya\t2\td", a.toString());
    }

    @Test
    public void testDifferentSizeKeys() {
        CompositeKey b = new BinaryKey("keya", "2");
        CompositeKey t = new TernaryKey("keya", "2", "d");
        assertFalse(b.equals(t));
        assertFalse(t.equals(b));
        assertEquals(1, t.compareTo(b));
        assertEquals(-1, b.compareTo(t));
    }
}
