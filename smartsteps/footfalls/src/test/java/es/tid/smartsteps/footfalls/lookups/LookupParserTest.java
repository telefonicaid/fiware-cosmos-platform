package es.tid.smartsteps.footfalls.lookups;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
public class LookupParserTest {

    private LookupParser parser;

    @Before
    public void setUp() {
        this.parser = new LookupParser("\t");
    }

    @Test
    public void testParse() {
        final Lookup lookup = this.parser.parse("assag43\t123\t0.57");
        assertEquals("assag43", lookup.getKey());
        assertEquals("123", lookup.getValue());
        assertEquals(0.57D, lookup.getProportion(), 0.0D);
    }

    @Test
    public void testAutoUnquote() {
        final Lookup lookup = this.parser.parse("\"assag43\"\t\"123\"\t0.57");
        assertEquals("assag43", lookup.getKey());
        assertEquals("123", lookup.getValue());
   }
}
