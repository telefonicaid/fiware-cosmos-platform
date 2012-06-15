package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;

/**
 *
 * @author dmicol
 */
public class CellToMicrogridLookupEntry implements LookupEntry {
    public final String cellId;
    public final String microgridId;
    public BigDecimal proportion;

    public CellToMicrogridLookupEntry(String cellId, String microgridId,
                                      BigDecimal proportion) {
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
    public BigDecimal getProportion() {
        return this.proportion;
    }
}
