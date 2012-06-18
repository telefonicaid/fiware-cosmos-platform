package es.tid.cosmos.mobility.parsing;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 *
 * @author sortega
 */
public class BtsParserTest {
    @Test
    public void testParse() throws Exception {
        BtsParser parser = new BtsParser("173600  173601 711.862 66737", "\\|");
        Bts bts = parser.parse();
        assertEquals(173600L, bts.getPlaceId());
        assertEquals(711.862D, bts.getArea(), 0.0D);
        assertEquals(66737L, bts.getComms());
    }
}
