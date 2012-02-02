package es.tid.ps.base.mapreduce;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
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
        assertTrue(b1.hashCode() == b2.hashCode());
        assertFalse(b1.equals(b3));
        assertTrue(b1.hashCode() == b3.hashCode());

        CompositeKey t1 = new TernaryKey("keya", "1", "b"),
                     t2 = new TernaryKey("keya", "1", "b"),
                     t3 = new TernaryKey("keya", "b", "1");
        assertFalse(t1.equals(new Object()));
        assertTrue(t1.equals(t2));
        assertTrue(t1.hashCode() == t2.hashCode());
        assertFalse(t1.equals(t3));
        assertTrue(t1.hashCode() == t3.hashCode());
        
        assertFalse(b1.equals(t1));
        assertFalse(t1.equals(b1));
    }
}
