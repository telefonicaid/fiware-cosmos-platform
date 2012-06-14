package es.tid.smartsteps.dispersion.data;

import java.io.StringReader;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.dispersion.parsing.CellToMicrogridEntryParser;
import es.tid.smartsteps.dispersion.parsing.MicrogridToPolygonEntryParser;

/**
 *
 * @author dmicol
 */
public class LookupTableTest {
    private LookupTable instance;
    
    @Before
    public void setUp() {
        this.instance = new LookupTable();
    }

    @Test
    public void testLoadCellToMicrogridEntry() throws Exception {
        String value = "assag43\t123\t0.57\nafsd43\t456\t0.31";
        this.instance.load(new StringReader(value),
                           new CellToMicrogridEntryParser("\t"));
        assertNotNull(this.instance.get("assag43"));
        assertNotNull(this.instance.get("afsd43"));
        assertNull(this.instance.get("a432fas"));
    }

    @Test
    public void testLoadMicrogridToPolygonEntry() throws Exception {
        String value = "assag43\tabc123\t0.57\nafsd43\tcde456\t0.31";
        this.instance.load(new StringReader(value),
                           new MicrogridToPolygonEntryParser("\t"));
        assertNotNull(this.instance.get("assag43"));
        assertNotNull(this.instance.get("afsd43"));
        assertNull(this.instance.get("a432fas"));
    }
}
