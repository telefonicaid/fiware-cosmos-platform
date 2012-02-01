package es.tid.ps.profile.categoryextraction;

import static org.junit.Assert.*;
import org.junit.Test;

/**
 * Test cases for CompositeKey
 * 
 * @author sortega
 */
public class CompositeKeyTest {
    
    @Test
    public void testEqualityAndHashCode() {
        CompositeKey a1 = new CompositeKey("keya", "1"),
                     a2 = new CompositeKey("keya", "1");
        
        assertFalse(a1.equals(new Object()));
        
        assertTrue(a1.equals(a2));
        assertTrue(a1.hashCode() == a2.hashCode());

        a2.setSecondaryKey("2");
        assertFalse(a1.equals(a2));
        assertTrue(a1.hashCode() != a2.hashCode());
    }

    @Test
    public void shouldBeOrderedByPrimaryKey() {
        CompositeKey a = new CompositeKey("keya", "3");
        CompositeKey b = new CompositeKey("keyb", "1");
        
        assertEquals(-1, a.compareTo(b));
        assertEquals( 1, b.compareTo(a));
        assertEquals( 0, a.compareTo(a));
    }
    
    @Test
    public void shouldBeOrderedBySecondaryKey() {
        CompositeKey a = new CompositeKey("keya", "3");
        CompositeKey a2 = new CompositeKey("keya", "2");        
        assertEquals(1, a.compareTo(a2));
    }
}
