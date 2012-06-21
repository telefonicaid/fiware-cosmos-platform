package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;

/**
 *
 * @author dmicol
 */
public class MicrogridToPolygonLookupEntry implements LookupEntry {
    public final String microgridId;
    public final String polygonId;
    public final BigDecimal proportion;

    public MicrogridToPolygonLookupEntry(String microgridId, String polygonId,
                                         BigDecimal proportion) {
        this.microgridId = microgridId;
        this.polygonId = polygonId;
        this.proportion = proportion;
    }
    
    @Override
    public String getKey() {
        return this.microgridId;
    }

    @Override
    public String getSecondaryKey() {
        return this.polygonId;
    }
    
    @Override
    public BigDecimal getProportion() {
        return this.proportion;
    }
}
