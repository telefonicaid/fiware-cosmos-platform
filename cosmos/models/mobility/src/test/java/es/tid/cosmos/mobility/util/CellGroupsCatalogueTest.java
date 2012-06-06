package es.tid.cosmos.mobility.util;

import java.io.IOException;
import java.io.StringReader;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.CellGroup;

/**
 *
 * @author dmicol
 */
public class CellGroupsCatalogueTest {
    @Test
    public void testLoadAndfilter() throws IOException {
        String text = "334305|1|2\n"
                + "334306|3|4\n"
                + "334307|5|6\n"
                + "334305|7|8";
        List<CellGroup> cellGroups = CellGroupsCatalogue.load(
                new StringReader(text));
        assertNotNull(cellGroups);
        assertEquals(4, cellGroups.size());
        assertEquals(3359493L, cellGroups.get(0).getCellId());
        assertEquals(3359494L, cellGroups.get(1).getCellId());
        assertEquals(3359495L, cellGroups.get(2).getCellId());
        assertEquals(3359493L, cellGroups.get(3).getCellId());
        
        List<CellGroup> filteredCellGroups = CellGroupsCatalogue.filter(
                cellGroups, 3359493L);
        assertEquals(2, filteredCellGroups.size());
        for (CellGroup cellGroup : filteredCellGroups) {
            assertEquals(3359493L, cellGroup.getCellId());
        }
    }
}
