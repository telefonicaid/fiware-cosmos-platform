package es.tid.bdp.mobility.data;


import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * User: masp20
 * Date: 27-ene-2012
 * Time: 13:18:12
 */
public class CellCatalogue {

    private static CellCatalogue INSTANCE = null;
    private Map<Long, Cell> catalogue;

    private CellCatalogue() {
        this.catalogue = new HashMap<Long, Cell>();
    }

    private static synchronized void createInstance() {
        if (CellCatalogue.INSTANCE == null) {
            CellCatalogue.INSTANCE = new CellCatalogue();
        }
    }

    public static CellCatalogue getInstance() {
        if (INSTANCE == null) {
            CellCatalogue.createInstance();
        }
        return CellCatalogue.INSTANCE;
    }

    public void addCell(final Cell cell) {
        if (cell != null) {
            this.catalogue.put(cell.getIdCell(), cell);
        }
    }

    public Cell getCell(final long cellId) {
        return this.catalogue.get(cellId);
    }

    public boolean containsCell(final long cellId) {
        return this.catalogue.containsKey(cellId);
    }

    public Iterator<Long> getKeysIterator() {
        return this.catalogue.keySet().iterator();
    }
}
