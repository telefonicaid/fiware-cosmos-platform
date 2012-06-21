package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class MicrogridToPolygonEntryTest {
    private MicrogridToPolygonLookupEntry instance;
    
    @Before
    public void setUp() {
        this.instance =
                new MicrogridToPolygonLookupEntry("123", "456",
                                                  new BigDecimal(0.57D));
    }

    @Test
    public void testGetKey() {
        assertEquals("123", this.instance.getKey());
    }

    @Test
    public void testGetProportion() {
        assertEquals(0.57D, this.instance.getProportion().doubleValue(), 0.0D);
    }
}
