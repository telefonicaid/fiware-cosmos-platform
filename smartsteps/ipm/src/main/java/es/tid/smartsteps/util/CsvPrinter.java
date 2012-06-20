package es.tid.smartsteps.util;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Collection;

/**
 * This interface defines the signature of an object capable of printing
 * generic objects in CSV format.
 */
public interface CsvPrinter<T> {

    /**
     * Print given object into given output stream
     * @param t the object to be printed
     * @param output the destination of the printing
     * @throws IOException when an IO error occurs in given output
     */
    void print(T t, OutputStream output) throws IOException;

    /**
     * Print given objects into given output stream
     * @param t the objects to be printed
     * @param output the destination of the printing
     * @throws IOException when an IO error occurs in given output
     */
    void print(T[] t, OutputStream output) throws IOException;

    /**
     * Print given objects into given output stream
     * @param t the objects to be printed
     * @param output the destination of the printing
     * @throws IOException when an IO error occurs in given output
     */
    void print(Collection<T> t, OutputStream output) throws IOException;
}
