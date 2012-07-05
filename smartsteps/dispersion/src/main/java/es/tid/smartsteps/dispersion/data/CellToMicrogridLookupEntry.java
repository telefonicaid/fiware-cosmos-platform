package es.tid.smartsteps.dispersion.data;

/**
 *
 * @author dmicol
 */
public class CellToMicrogridLookupEntry implements LookupEntry {

    public final String cellId;
    public final String microgridId;
    public double proportion;

    public CellToMicrogridLookupEntry(String cellId, String microgridId,
                                      double proportion) {
        this.cellId = cellId;
        this.microgridId = microgridId;
        this.proportion = proportion;
    }
    
    @Override
    public String getKey() {
        return this.cellId;
    }

    @Override
    public String getSecondaryKey() {
        return this.microgridId;
    }

    @Override
    public double getProportion() {
        return this.proportion;
    }
}
