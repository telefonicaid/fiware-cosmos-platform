package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;

import static es.tid.smartsteps.util.InetIpmUtil.FieldIndex.*;
/**
 *
 * @author dmicol
 */
public abstract class InetIpmUtil {
    private static final char DELIMITER = '|';

    public static enum FieldIndex {
        TYPE            (0),
        CALL_TYPE       (1),
        ANONYMISED_IMSI (2),
        FIRST_TEMP_IMSI (3),
        LAST_TEMP_IMSI  (4),
        IMEI_TAC        (5),
        ANONYMISED_IMEI (6),
        LACOD           (7),
        CELL_ID         (8),
        EVENT_DATE_TIME (9),
        DTAP_CAUSE      (10),
        BSS_MAP_CAUSE   (11),
        CC_CAUSE        (12),
        MM_CAUSE        (13),
        RANAP_CAUSE     (14);

        public int value;

        private FieldIndex(int value) {
            this.value = value;
        }
    }
    
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
        if (fields.length != FieldIndex.values().length) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return InetIpm.newBuilder()
                .setType(fields[TYPE.value])
                .setCallType(fields[CALL_TYPE.value])
                .setAnonymisedImsi(fields[ANONYMISED_IMSI.value])
                .setFirstTempImsi(fields[FIRST_TEMP_IMSI.value])
                .setLastTempImsi(fields[LAST_TEMP_IMSI.value])
                .setImeiTac(fields[IMEI_TAC.value])
                .setAnonymisedImei(fields[ANONYMISED_IMEI.value])
                .setLacod(fields[LACOD.value])
                .setCellId(fields[CELL_ID.value])
                .setEventDateTime(fields[EVENT_DATE_TIME.value])
                .setDtapCause(fields[DTAP_CAUSE.value])
                .setBssmapCause(fields[BSS_MAP_CAUSE.value])
                .setCcCause(fields[CC_CAUSE.value])
                .setMmCause(fields[MM_CAUSE.value])
                .setRanapCause(fields[RANAP_CAUSE.value])
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
