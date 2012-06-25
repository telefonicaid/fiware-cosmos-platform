package es.tid.smartsteps.util;

import java.nio.charset.Charset;

/**
 * Parent class for CSV parsers. This abstract class encapsulates the
 * properties common to any CSV parser.
 *
 * @author apv
 */
public abstract class AbstractCsvParser<T> implements CsvParser<T> {

    private final String delimiter;
    private final Charset charset;

    /**
     * Create a new abstract CSV parser with given properties.
     * @param delimiter the delimiter used to distinguish columns while parsing
     * @param charset the charset used to decode characters from the input
     *                stream
     */
    protected AbstractCsvParser(String delimiter, Charset charset) {
        this.delimiter = delimiter;
        this.charset = charset;
    }

    public String getDelimiter() {
        return this.delimiter;
    }

    public Charset getCharset() {
        return charset;
    }
}
