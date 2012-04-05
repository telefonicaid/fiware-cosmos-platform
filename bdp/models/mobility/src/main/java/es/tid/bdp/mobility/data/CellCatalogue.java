package es.tid.bdp.mobility.data;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class CellCatalogue {
    private Map<Long, Cell> catalogue;

    CellCatalogue() {
        this.catalogue = new HashMap<Long, Cell>();
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
