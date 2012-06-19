package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;

import static es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw.*;
/**
 *
 * @author dmicol
 */
public abstract class InetRawUtil {
    private static final char DELIMITER = '|';

    public static InetRaw create(String type, String callType,
            String imsi, String firstTempImsi, String lastTempImsi,
            String imei, String lacod, String cellId,
            String eventDateTime, String dtapCause, String bssmapCause,
            String ccCause, String mmCause, String ranapCause) {
        return InetRaw.newBuilder()
                .setType(type)
                .setCallType(callType)
                .setImsi(imsi)
                .setFirstTempImsi(firstTempImsi)
                .setLastTempImsi(lastTempImsi)
                .setImei(imei)
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
    
    public static InetRaw parse(String line) throws ParseException {
        String[] fields = line.split("\\" + DELIMITER);
        if (fields.length != InetRaw.getDescriptor().getFields().size()) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return InetRaw.newBuilder()
                .setType(fields[TYPE_FIELD_NUMBER - 1])
                .setCallType(fields[CALLTYPE_FIELD_NUMBER - 1])
                .setImsi(fields[IMSI_FIELD_NUMBER - 1])
                .setFirstTempImsi(fields[FIRSTTEMPIMSI_FIELD_NUMBER - 1])
                .setLastTempImsi(fields[LASTTEMPIMSI_FIELD_NUMBER - 1])
                .setImei(fields[IMEI_FIELD_NUMBER - 1])
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
    
    public static String toString(InetRaw inetRaw) {
        return (inetRaw.getType() + DELIMITER
                + inetRaw.getCallType() + DELIMITER
                + inetRaw.getImsi() + DELIMITER
                + inetRaw.getFirstTempImsi() + DELIMITER
                + inetRaw.getLastTempImsi() + DELIMITER
                + inetRaw.getImei() + DELIMITER
                + inetRaw.getLacod() + DELIMITER
                + inetRaw.getCellId() + DELIMITER
                + inetRaw.getEventDateTime() + DELIMITER
                + inetRaw.getDtapCause() + DELIMITER
                + inetRaw.getBssmapCause() + DELIMITER
                + inetRaw.getCcCause() + DELIMITER
                + inetRaw.getMmCause() + DELIMITER
                + inetRaw.getRanapCause());
    }
}
