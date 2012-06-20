package es.tid.smartsteps.ipm;

import java.nio.charset.Charset;

/**
 */
public abstract class AbstractRawToIpmConverter implements RawToIpmConverter {

    private String delimiter;
    private Charset charset;

    protected AbstractRawToIpmConverter(String delimiter, Charset charset) {
        this.delimiter = delimiter;
        this.charset = charset;
    }

    public String getDelimiter() {
        return delimiter;
    }

    public Charset getCharset() {
        return charset;
    }
}
