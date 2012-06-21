package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;
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
        this.instance = new CellToMicrogridLookupEntry("abc", "def",
                                                       new BigDecimal(0.37D));
    }

    @Test
    public void testGetKey() {
        assertEquals("abc", this.instance.getKey());
    }

    @Test
    public void testGetProportion() {
        assertEquals(0.37D, this.instance.getProportion().doubleValue(), 0.0D);
    }
}
