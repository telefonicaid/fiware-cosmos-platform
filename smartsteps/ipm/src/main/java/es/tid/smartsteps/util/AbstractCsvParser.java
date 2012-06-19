package es.tid.smartsteps.util;

import java.nio.charset.Charset;

/**
 */
public abstract class AbstractCsvParser<T> implements CsvParser<T> {

    private char delimiter;
    private Charset charset;

    protected AbstractCsvParser(char delimiter, Charset charset) {
        this.delimiter = delimiter;
        this.charset = charset;
    }

    public char getDelimiter() {
        return this.delimiter;
    }

    public Charset getCharset() {
        return charset;
    }
}
