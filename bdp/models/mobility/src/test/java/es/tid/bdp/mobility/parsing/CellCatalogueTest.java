package es.tid.bdp.mobility.parsing;

import java.util.Iterator;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.mobility.data.Cell;
import es.tid.bdp.mobility.data.CellCatalogue;

/**
 * @author masp20
 */
public class CellCatalogueTest {
    private CellCatalogue cellCatalogue;


    @Before
    public void setUp() {
        this.cellCatalogue = CellCatalogue.getInstance();
    }

    @Test
    public void testCreateCellCatalogue() {
        assertNotNull("encode/decode Fail", this.cellCatalogue);
    }

    @Test
    public void testAddCell2Catalogue() {
        final Cell testCell = createTestCell(1L);

        this.cellCatalogue.addCell(testCell);

        final Cell cell = this.cellCatalogue.getCell(testCell.getIdCell());
        assertEquals("Incorrect Cell Retrieved", testCell, cell);

    }

    @Test
    public void testContainsCell() {
        final Cell testCell = createTestCell(1L);

        this.cellCatalogue.addCell(testCell);

        final boolean resultTrue = this.cellCatalogue.containsCell(testCell.getIdCell());
        assertTrue("Cell Not Found", resultTrue);

        final boolean resultFalse = this.cellCatalogue.containsCell(10);
        assertFalse("Cell Incorrect Found", resultFalse);
    }


    @Test
    public void testAddMultipeleCells() {
        final Cell testCell = createTestCell(1);
        final Cell testCell2 = createTestCell(2);
        final Cell testCell3 = createTestCell(3);

        boolean testCell1Found = false;
        boolean testCell2Found = false;
        boolean testCell3Found = false;

        this.cellCatalogue.addCell(testCell);
        this.cellCatalogue.addCell(testCell2);
        this.cellCatalogue.addCell(testCell3);

        final Iterator<Long> iterator = this.cellCatalogue.getKeysIterator();
        while (iterator.hasNext()) {
            final Long cellId = iterator.next();
            if (cellId == testCell.getIdCell()) {
                testCell1Found = true;
            } else if (cellId == testCell2.getIdCell()) {
                testCell2Found = true;
            } else if (cellId == testCell3.getIdCell()) {
                testCell3Found = true;
            }
        }

        assertTrue("Cell Not Found", testCell1Found);
        assertTrue("Cell Not Found", testCell2Found);
        assertTrue("Cell Not Found", testCell3Found);

        final boolean resultFalse = this.cellCatalogue.containsCell(10);
        assertFalse("Cell Incorrect Found", resultFalse);

    }

    private Cell createTestCell(final long idCell) {
        final Cell cell = new Cell();
        cell.setIdCell(idCell);

        cell.setGeoLocationLevel1(2 * idCell);
        cell.setGeoLocationLevel2(3 * idCell);
        cell.setGeoLocationLevel3(4 * idCell);
        cell.setGeoLocationLevel4(5 * idCell);

        cell.setLatitude(1.0 * idCell);
        cell.setLongitude(-1.0 * idCell);
        return cell;
    }

}
