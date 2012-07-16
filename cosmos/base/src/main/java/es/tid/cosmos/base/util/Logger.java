package es.tid.cosmos.base.util;

/**
 *
 * @author dmicol
 */
public final class Logger {
    private Logger() {
    }

    public static org.apache.log4j.Logger get(Class clazz) {
        return org.apache.log4j.Logger.getLogger(clazz);
    }
}
