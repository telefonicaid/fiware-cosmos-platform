package es.tid.cosmos.base.util;

/**
 *
 * @author dmicol
 */
public final class Logger {
    private static org.apache.log4j.Logger logger;
    
    private Logger() {
    }
    
    public static org.apache.log4j.Logger get(Class clazz) {
        if (logger == null) {
            logger = org.apache.log4j.Logger.getLogger(clazz);
        }
        return logger;
    }
}
