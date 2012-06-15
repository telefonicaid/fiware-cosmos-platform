package es.tid.smartsteps.dispersion.data;

import java.util.List;

/**
 * Entry
 *
 * represents an entry in a table, therefore a class where one of its fields is
 * a primary key
 *
 * @author dmicol, logc
 */
public interface Entry {
    /**
     * Gets the main (primary) key of this entry.
     *
     * @return The corresponding key.
     */
    String getKey();


}
