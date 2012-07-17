package es.tid.smartsteps.footfalls.microgrids.parsing;

import java.io.IOException;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.footfalls.microgrids.CatchmentsBasedTest;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;

/**
 *
 * @author dmicol
 */
public class CatchmentsParserTest extends CatchmentsBasedTest {

    CatchmentsParser instance;

    public CatchmentsParserTest() throws IOException {}

    @Before
    public void setUp() {
        this.instance = new CatchmentsParser();
    }

    @Test
    public void testParse() {
        final Catchments parsedCatchments = this.instance.parse(this.catchments);
        assertEquals("000012006440", parsedCatchments.getId());
        assertEquals("20120424", parsedCatchments.getDate());
        assertEquals(54.813522D, parsedCatchments.getLatitude(), 0.0D);
        assertEquals(-1.845595, parsedCatchments.getLongitude(), 0.0D);
        assertEquals(5, parsedCatchments.getCatchmentsCount());
    }
}
