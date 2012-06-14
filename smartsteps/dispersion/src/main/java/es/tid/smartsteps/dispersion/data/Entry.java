package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;

/**
 *
 * @author dmicol
 */
public interface Entry {
    /**
     * Gets the main (primary) key of this entry.
     *
     * @return The corresponding key.
     */
    String getKey();

    /**
     * Gets the proportion that this entry informs for scaling, if any.
     *
     * @return The corresponding proportion.
     */
    BigDecimal getProportion();
}
