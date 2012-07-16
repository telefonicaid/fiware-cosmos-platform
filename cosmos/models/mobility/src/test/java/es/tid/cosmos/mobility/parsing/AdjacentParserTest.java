package es.tid.cosmos.mobility.parsing;

import org.junit.Test;
import static org.junit.Assert.assertEquals;

import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjacentParserTest {

    @Test
    public void testParse() {
        AdjacentParser parser = new AdjacentParser("123|456", "|");
        TwoInt obj = parser.parse();
        assertEquals(123, obj.getNum1());
        assertEquals(456, obj.getNum2());
    }
}
