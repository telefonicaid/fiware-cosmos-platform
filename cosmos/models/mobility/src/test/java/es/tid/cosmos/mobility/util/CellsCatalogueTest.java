package es.tid.cosmos.mobility.util;

import java.io.IOException;
import java.io.StringReader;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class CellsCatalogueTest {
    @Test
    public void testLoad() throws IOException {
        String text = "334305|2221436242|12|34|56|78\n"
                + "334306|2221436242|12|34|56|78\n"
                + "334307|2221436242|12|34|56|78";
        List<Cell> cells = CellsCatalogue.load(new StringReader(text));
        assertNotNull(cells);
        assertEquals(3, cells.size());
        assertEquals(3359493L, cells.get(0).getCellId());
        assertEquals(3359494L, cells.get(1).getCellId());
        assertEquals(3359495L, cells.get(2).getCellId());
    }
}
