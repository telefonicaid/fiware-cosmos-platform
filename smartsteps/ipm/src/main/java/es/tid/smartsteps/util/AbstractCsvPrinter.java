package es.tid.smartsteps.util;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.nio.charset.Charset;
import java.util.Collection;
import java.util.Iterator;

import static java.util.Arrays.asList;

/**
 * Abstract class implementing the {@link CsvPrinter} interface. This
 * abstract class encapsulates the common properties to all CSV printer
 * objects.
 *
 * @author apv
 */
public abstract class AbstractCsvPrinter<T> implements CsvPrinter<T> {

    private final String delimiter;
    private final Charset charset;

    /**
     * Create a new abstract CSV printer with given properties.
     * @param delimiter the delimiter used to separate columns
     * @param charset the charse used to print characters
     */
    protected AbstractCsvPrinter(String delimiter, Charset charset) {
        this.delimiter = delimiter;
        this.charset = charset;
    }

    public String getDelimiter() {
        return delimiter;
    }

    public Charset getCharset() {
        return charset;
    }

    @Override
    public void print(T[] t, OutputStream output) throws IOException {
        this.print(asList(t), output);
    }

    @Override
    public void print(Collection<T> t, OutputStream output) throws IOException {
        Iterator<T> it = t.iterator();
        while (it.hasNext()) {
            T elem = it.next();
            this.print(elem, output);
            if (it.hasNext()) {
                BufferedWriter outputWriter = new BufferedWriter(
                        new OutputStreamWriter(output,this.charset));
                outputWriter.newLine();
                outputWriter.close();
            }
        }
    }
}
