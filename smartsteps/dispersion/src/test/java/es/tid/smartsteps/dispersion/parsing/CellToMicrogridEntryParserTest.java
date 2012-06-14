package es.tid.smartsteps.dispersion.parsing;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.dispersion.data.CellToMicrogridEntry;

/**
 *
 * @author dmicol
 */
public class CellToMicrogridEntryParserTest {
    private CellToMicrogridEntryParser parser;
    
    @Before
    public void setUp() {
        this.parser = new CellToMicrogridEntryParser("\t");
    }

    @Test
    public void testParse() {
        CellToMicrogridEntry entry = this.parser.parse("assag43\t123\t0.57");
        assertEquals("assag43", entry.cellId);
        assertEquals("123", entry.microgridId);
        assertEquals(0.57D, entry.proportion.doubleValue(), 0.0D);
    }
}
