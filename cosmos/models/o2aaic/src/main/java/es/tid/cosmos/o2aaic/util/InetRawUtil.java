package es.tid.cosmos.o2aaic.util;

import es.tid.cosmos.o2aaic.data.InetRaw;

/**
 *
 * @author dmicol
 */
public abstract class InetRawUtil {
    private static final String DELIMITER = "\\|";
    
    private InetRawUtil() {
    }
    
    public static InetRaw parse(String line) {
        String[] fields = line.split(DELIMITER);
        return new InetRaw(fields[0], fields[1], fields[2], fields[3],
                fields[4], fields[5], fields[6], fields[7], fields[8],
                fields[9], fields[10], fields[11], fields[12], fields[13]);
    }
    
    public static String toString(InetRaw inetRaw) {
        return (inetRaw.getType() + DELIMITER + inetRaw.getCallType()
                + DELIMITER + inetRaw.getImsi() + DELIMITER
                + inetRaw.getFirstTempImsi() + DELIMITER
                + inetRaw.getLastTempImsi() + DELIMITER + inetRaw.getImei()
                + DELIMITER + inetRaw.getLacod() + DELIMITER
                + inetRaw.getCellId() + DELIMITER + inetRaw.getEventDateTime()
                + DELIMITER + inetRaw.getDtapCause() + DELIMITER
                + inetRaw.getBssmapCause() + DELIMITER + inetRaw.getCcCause()
                + DELIMITER + inetRaw.getMmCause() + DELIMITER
                + inetRaw.getRanapCause());
    }
}
