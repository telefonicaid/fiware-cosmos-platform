package es.tid.smartsteps.footfalls.trafficcounts;

import java.io.IOException;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.footfalls.config.Config;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
public class TrafficCountsParserTest extends TrafficCountsBasedTest {

    private TrafficCountsParser parser;

    public TrafficCountsParserTest() throws IOException {
    }

    @Before
    public void setUp() throws IOException {
        this.parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
    }

    @Test
    public void testParse() {
        final TrafficCounts counts = this.parser.parse(this.trafficCounts);
        assertEquals("20120527", counts.getDate());
    }

    public void testInvalidEntry() {
        assertNull(this.parser.parse("blahblah"));
    }
}
