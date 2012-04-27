package es.tid.cosmos.mobility.util;

import es.tid.cosmos.mobility.MobilityMain;

/**
 *
 * @author dmicol
 */
public final class Logger {
    private static final org.apache.log4j.Logger LOGGER =
            org.apache.log4j.Logger.getLogger(MobilityMain.class);
    
    private Logger() {
    }
    
    public static org.apache.log4j.Logger get() {
        return LOGGER;
    }
}
