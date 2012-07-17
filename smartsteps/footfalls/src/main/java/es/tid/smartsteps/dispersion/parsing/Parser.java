package es.tid.smartsteps.dispersion.parsing;

/**
 *
 * @author dmicol
 */
public abstract class Parser<E> {

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
}
