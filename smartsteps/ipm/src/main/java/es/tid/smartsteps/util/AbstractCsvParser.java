package es.tid.smartsteps.util;

import java.nio.charset.Charset;

/**
 */
public abstract class AbstractCsvParser<T> implements CsvParser<T> {

    private String delimiter;
    private Charset charset;

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
