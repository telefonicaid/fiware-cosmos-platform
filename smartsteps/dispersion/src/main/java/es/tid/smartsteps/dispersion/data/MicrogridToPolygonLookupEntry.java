package es.tid.smartsteps.dispersion.data;

/**
 *
 * @author dmicol
 */
public class MicrogridToPolygonLookupEntry implements LookupEntry {

    public final String microgridId;
    public final String polygonId;
    public final double proportion;

    public MicrogridToPolygonLookupEntry(String microgridId, String polygonId,
                                         double proportion) {
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
    public double getProportion() {
        return this.proportion;
    }
}
