package es.tid.ps.mobility.voronoi;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.Reader;
import java.io.StringReader;
import static java.util.Arrays.asList;
import static java.util.Collections.EMPTY_LIST;

import static org.junit.Assert.*;
import org.junit.Test;

import es.tid.ps.mobility.data.BtsUtil;
import es.tid.ps.mobility.data.MobProtocol.Bts;
import es.tid.ps.mobility.data.MobProtocol.Bts.Builder;

/**
 *
 * @author sortega
 */
public class ImportBtsToolTest {

    @Test
    public void testImport() throws Exception {
        Reader cellCatalogue = new StringReader(
                "13401|159300|2423310\n" +
                "13402|158468|2422904\n" +
                "13403|159389|2422585");
        Reader adjacentBts = new StringReader(
                "13401|13402\n" +
                "13402|13401\n");
        Reader areas = new StringReader(
                "13401|13401|70.261|123459\n" +
                "13402|13402|20.1|334555\n" +
                "13403|13403|0.78|456666");
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        ImportBtsTool instance = new ImportBtsTool(cellCatalogue, adjacentBts,
                areas, output);
        instance.run();

        ByteArrayInputStream input = new ByteArrayInputStream(output.toByteArray());

        Builder bts13401 = Bts.newBuilder();
        assertTrue(bts13401.mergeDelimitedFrom(input));
        assertEquals(BtsUtil.create(13401, 159300, 2423310, 70.261, asList(13402L)),
                bts13401.build());

        Builder bts13402 = Bts.newBuilder();
        assertTrue(bts13402.mergeDelimitedFrom(input));
        assertEquals(BtsUtil.create(13402, 158468, 2422904, 20.1, asList(13401L)),
                bts13402.build());

        Builder bts13403 = Bts.newBuilder();
        assertTrue(bts13403.mergeDelimitedFrom(input));
        assertEquals(BtsUtil.create(13403, 159389, 2422585, 0.78, EMPTY_LIST),
                bts13403.build());

        assertFalse(Bts.newBuilder().mergeDelimitedFrom(input));
    }
}
