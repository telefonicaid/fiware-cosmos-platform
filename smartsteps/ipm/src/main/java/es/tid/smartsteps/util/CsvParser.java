package es.tid.smartsteps.util;

import java.io.InputStream;

import es.tid.smartsteps.ipm.ParseException;

/**
 */
public interface CsvParser<T> {

    T parse(InputStream input) throws ParseException;
}
