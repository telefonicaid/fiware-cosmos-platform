package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;

import static es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm.*;

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
        if (fields.length != InetIpm.getDescriptor().getFields().size()) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return InetIpm.newBuilder()
                .setType(fields[TYPE_FIELD_NUMBER - 1])
                .setCallType(fields[CALLTYPE_FIELD_NUMBER - 1])
                .setAnonymisedImsi(fields[ANONYMISEDIMSI_FIELD_NUMBER - 1])
                .setFirstTempImsi(fields[FIRSTTEMPIMSI_FIELD_NUMBER - 1])
                .setLastTempImsi(fields[LASTTEMPIMSI_FIELD_NUMBER - 1])
                .setImeiTac(fields[IMEITAC_FIELD_NUMBER - 1])
                .setAnonymisedImei(fields[ANONYMISEDIMEI_FIELD_NUMBER - 1])
                .setLacod(fields[LACOD_FIELD_NUMBER - 1])
                .setCellId(fields[CELLID_FIELD_NUMBER - 1])
                .setEventDateTime(fields[EVENTDATETIME_FIELD_NUMBER - 1])
                .setDtapCause(fields[DTAPCAUSE_FIELD_NUMBER - 1])
                .setBssmapCause(fields[BSSMAPCAUSE_FIELD_NUMBER - 1])
                .setCcCause(fields[CCCAUSE_FIELD_NUMBER - 1])
                .setMmCause(fields[MMCAUSE_FIELD_NUMBER - 1])
                .setRanapCause(fields[RANAPCAUSE_FIELD_NUMBER - 1])
                .build();
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
