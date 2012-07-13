package es.tid.smartsteps.util;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import es.tid.smartsteps.ipm.ParseException;

/**
 * A generic interface for any object capable to parse CSV lines. The
 * generic parameter indicates the type resulting of parsing.
 *
 * @author apv
 */
public interface CsvParser<T> {

    /**
     * Parse an input comprised by an arbitrary number of CSV lines and
     * return the resulting T objects.
     *
     * @param input the input stream to read lines CSV from
     * @return the list of parsed objects
     * @throws ParseException when a parsing error occurs
     */
    List<T> parse(InputStream input) throws ParseException, IOException;
}
