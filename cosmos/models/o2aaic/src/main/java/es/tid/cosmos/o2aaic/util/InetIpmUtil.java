package es.tid.cosmos.o2aaic.util;

import es.tid.cosmos.o2aaic.data.InetIpm;

/**
 *
 * @author dmicol
 */
public abstract class InetIpmUtil {
    private static final String DELIMITER = "\\|";
    
    private InetIpmUtil() {
    }
    
    public static InetIpm parse(String line) {
        String[] fields = line.split(DELIMITER);
        return new InetIpm(fields[0], fields[1], fields[2], fields[3],
                fields[4], fields[5], fields[6], fields[7], fields[8],
                fields[9], fields[10], fields[11], fields[12], fields[13],
                fields[14]);
    }
    
    public static String toString(InetIpm inetIpm) {
        return (inetIpm.getType() + DELIMITER + inetIpm.getCallType()
                + DELIMITER + inetIpm.getAnonymisedImsi() + DELIMITER
                + inetIpm.getFirstTempImsi() + DELIMITER
                + inetIpm.getLastTempImsi() + DELIMITER + inetIpm.getImeiTac()
                + DELIMITER + inetIpm.getAnonymisedImei() + DELIMITER
                + inetIpm.getLacod() + DELIMITER + inetIpm.getCellId()
                + DELIMITER + inetIpm.getEventDateTime() + DELIMITER
                + inetIpm.getDtapCause() + DELIMITER + inetIpm.getBssmapCause()
                + DELIMITER + inetIpm.getCcCause() + DELIMITER
                + inetIpm.getMmCause() + DELIMITER + inetIpm.getRanapCause());
    }
}
