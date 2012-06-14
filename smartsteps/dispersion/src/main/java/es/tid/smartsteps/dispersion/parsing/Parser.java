package es.tid.smartsteps.dispersion.parsing;

import es.tid.smartsteps.dispersion.data.Entry;

/**
 *
 * @author dmicol
 */
public abstract class Parser<E extends Entry> {
    protected final String delimiter;
    
    public Parser(String delimiter) {
        this.delimiter = delimiter;
    }
    
    /**
     * Parses a line. May throw an exception if the operation fails.
     * @param line The line to parse.
     * @return The parsed object.
     */
    public abstract E parse(String line);
    
    /**
     * Parses a line. Will not throw an exception if the operation fails.
     * @param line The line to parse.
     * @return The parsed object, or null if the operation failed.
     */
    public abstract E safeParse(String line);
}
 