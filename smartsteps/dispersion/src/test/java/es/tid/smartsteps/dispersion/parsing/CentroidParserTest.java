package es.tid.smartsteps.dispersion.parsing;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.SOACentroid;

/**
 *
 * @author dmicol
 */
public class CentroidParserTest {

    private CentroidParser parser;
    
    @Before
    public void setUp() {
        this.parser = new CentroidParser("\t");
    }

    @Test
    public void testParse() {
        final SOACentroid soaCentroid = this.parser.parse("abc\t0.3\t0.5\t0.4");
        assertEquals("abc", soaCentroid.getSoaId());
        assertEquals(0.3D, soaCentroid.getSquaredKilometers(), 0.0D);
        assertEquals(0.5D, soaCentroid.getLongitude(), 0.0D);
        assertEquals(0.4D, soaCentroid.getLatitude(), 0.0D);
    }
}
