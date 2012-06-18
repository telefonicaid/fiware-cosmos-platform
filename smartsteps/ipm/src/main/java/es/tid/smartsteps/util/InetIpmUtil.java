package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;

/**
 *
 * @author dmicol
 */
public abstract class InetIpmUtil {
    private static final char DELIMITER = '|';
    
    private InetIpmUtil() {
    }
    
    public static InetIpm create(String type, String callType,
            String anonymisedImsi, String firstTempImsi, String lastTempImsi,
            String imeiTac, String anonymisedImei, String lacod, String cellId,
            String eventDateTime, String dtapCause, String bssmapCause,
            String ccCause, String mmCause, String ranapCause) {
        return InetIpm.newBuilder()
                .setType(type)
                .setCallType(callType)
                .setAnonymisedImsi(anonymisedImsi)
                .setFirstTempImsi(firstTempImsi)
                .setLastTempImsi(lastTempImsi)
                .setImeiTac(imeiTac)
                .setAnonymisedImei(anonymisedImei)
                .setLacod(lacod)
                .setCellId(cellId)
                .setEventDateTime(eventDateTime)
                .setDtapCause(dtapCause)
                .setBssmapCause(bssmapCause)
                .setCcCause(ccCause)
                .setMmCause(mmCause)
                .setRanapCause(ranapCause)
                .build();
    }
    
    public static InetIpm parse(String line) throws ParseException {
        String[] fields = line.split("\\" + DELIMITER);
        if (fields.length != 15)
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        return create(fields[0], fields[1], fields[2], fields[3], fields[4],
                fields[5], fields[6], fields[7], fields[8], fields[9],
                fields[10], fields[11], fields[12], fields[13], fields[14]);
    }
    
    public static String toString(InetIpm inetIpm) {
        return (inetIpm.getType() + DELIMITER
                + inetIpm.getCallType() + DELIMITER
                + inetIpm.getAnonymisedImsi() + DELIMITER
                + inetIpm.getFirstTempImsi() + DELIMITER
                + inetIpm.getLastTempImsi() + DELIMITER
                + inetIpm.getImeiTac() + DELIMITER
                + inetIpm.getAnonymisedImei() + DELIMITER
                + inetIpm.getLacod() + DELIMITER
                + inetIpm.getCellId() + DELIMITER
                + inetIpm.getEventDateTime() + DELIMITER
                + inetIpm.getDtapCause() + DELIMITER
                + inetIpm.getBssmapCause() + DELIMITER
                + inetIpm.getCcCause() + DELIMITER
                + inetIpm.getMmCause() + DELIMITER
                + inetIpm.getRanapCause());
    }
}
