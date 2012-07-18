package es.tid.smartsteps.footfalls.catchments;

import java.io.IOException;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.footfalls.catchments.CatchmentsBasedTest;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;

/**
 *
 * @author dmicol
 */
public class CatchmentsParserTest extends CatchmentsBasedTest {

    private CatchmentsParser instance;
    private Catchments parsedCatchments;

    public CatchmentsParserTest() throws IOException {}

    @Before
    public void setUp() {
        this.instance = new CatchmentsParser();
        this.parsedCatchments = this.instance.parse(this.catchments);
    }

    @Test
    public void testParse() throws Exception {
        assertEquals("000012006440", this.parsedCatchments.getId());
        assertEquals("20120424", this.parsedCatchments.getDate());
        assertEquals(54.813522D, this.parsedCatchments.getLatitude(), 0.0D);
        assertEquals(-1.845595, this.parsedCatchments.getLongitude(), 0.0D);
        assertEquals(5, this.parsedCatchments.getCatchmentsCount());
    }

    @Test
    public void testToJson() throws Exception {
        String roundtripCatchments = this.instance
                .toJSON(this.parsedCatchments, true)
                .toString();
        String expectedOutput = this.catchments.replaceAll("\\s", "");
        assertEquals(expectedOutput, roundtripCatchments);
    }
}
