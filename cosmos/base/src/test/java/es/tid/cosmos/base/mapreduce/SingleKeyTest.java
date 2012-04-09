package es.tid.cosmos.base.mapreduce;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 * Test case for SingleKey
 *
 * @author dmicol
 */
public class SingleKeyTest {
    @Test
    public void testEqualityAndHashCode() {
        CompositeKey s1 = new SingleKey("keya"),
                     s2 = new SingleKey("keya");
        assertFalse(s1.equals(new Object()));
        assertTrue(s1.equals(s2));
        assertEquals(s1.hashCode(), s2.hashCode());
    }

    @Test
    public void shouldBeOrderedByPrimaryKey() {
        CompositeKey a = new SingleKey("keya");
        CompositeKey b = new SingleKey("keyb");
        assertEquals(-1, a.compareTo(b));
        assertEquals( 1, b.compareTo(a));
        assertEquals( 0, a.compareTo(a));
    }

    @Test
    public void testGettersAndSetters() {
        SingleKey s1 = new SingleKey("keya");
        assertEquals("keya", s1.getKey());

        SingleKey t2 = new SingleKey();
        t2.setKey("keya");
        
        assertTrue(s1.equals(t2));
    }
}
