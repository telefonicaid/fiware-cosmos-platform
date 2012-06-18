package es.tid.cosmos.mobility.parsing;

import org.junit.Test;
import static org.junit.Assert.assertEquals;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class CellParserTest {
    @Test
    public void testParse() {
        CellParser parser = new CellParser(
                "334305216764|2221436242|12|34|56|78", "\\|");
        Cell obj = parser.parse();
        assertEquals(86075236L, obj.getCellId());
        assertEquals(2221436242L, obj.getBts());
        assertEquals(12, obj.getGeoloc1());
        assertEquals(34, obj.getGeoloc2());
        assertEquals(56, (int)obj.getPosx());
        assertEquals(78, (int)obj.getPosy());
    }
}
