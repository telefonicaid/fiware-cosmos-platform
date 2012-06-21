package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;

/**
 * Represents an entry in a lookup table, where the lookup helps to join
 * a primary key to a secondary key.
 *
 * @author dmicol, logc
 */
public interface LookupEntry extends Entry {
    /**
     * Gets the secondary key of this entry.
     *
     * @return The corresponding key
     */
    String getSecondaryKey();

    /**
     * Gets the proportion that this entry informs for scaling, if any.
     *
     * @return The corresponding proportion.
     */
    BigDecimal getProportion();
}
