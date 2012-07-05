package es.tid.smartsteps.dispersion.data;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class CellToMicrogridEntryTest {

    private CellToMicrogridLookupEntry instance;
    
    @Before
    public void setUp() {
        this.instance = new CellToMicrogridLookupEntry("abc", "def", 0.37D);
    }

    @Test
    public void testGetKey() {
        assertEquals("abc", this.instance.getKey());
    }

    @Test
    public void testGetProportion() {
        assertEquals(0.37D, this.instance.getProportion(), 0.0D);
    }
}
