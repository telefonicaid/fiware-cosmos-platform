package es.tid.smartsteps.dispersion.parsing;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.dispersion.data.MicrogridToPolygonLookupEntry;

/**
 *
 * @author dmicol
 */
public class MicrogridToPolygonEntryParserTest {
    private MicrogridToPolygonEntryParser parser;
    
    @Before
    public void setUp() {
        this.parser = new MicrogridToPolygonEntryParser("\t");
    }

    @Test
    public void testParse() {
        MicrogridToPolygonLookupEntry entry = this.parser.parse("456\t123\t0.57");
        assertEquals("456", entry.microgridId);
        assertEquals("123", entry.polygonId);
        assertEquals(0.57D, entry.proportion.doubleValue(), 0.0D);
    }
}
