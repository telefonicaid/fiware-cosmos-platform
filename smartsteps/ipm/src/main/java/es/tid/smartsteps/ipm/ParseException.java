package es.tid.smartsteps.ipm;

/**
 * An exception class used to indicate an error while parsing raw data.
 *
 * @author apv
 */
public class ParseException extends Exception {

    public ParseException(String msg) {
        super(msg);
    }

    public ParseException(String msg, Throwable cause) {
        super(msg, cause);
    }
}
