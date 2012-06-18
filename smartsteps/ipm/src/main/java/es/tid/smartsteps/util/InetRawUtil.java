package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;

import static es.tid.smartsteps.util.InetRawUtil.FieldIndex.*;
/**
 *
 * @author dmicol
 */
public abstract class InetRawUtil {
    private static final char DELIMITER = '|';

    public static enum FieldIndex {
        TYPE            (0),
        CALL_TYPE       (1),
        IMSI            (2),
        FIRST_TEMP_IMSI (3),
        LAST_TEMP_IMSI  (4),
        IMEI            (5),
        LACOD           (6),
        CELL_ID         (7),
        EVENT_DATE_TIME (8),
        DTAP_CAUSE      (9),
        BSS_MAP_CAUSE   (10),
        CC_CAUSE        (11),
        MM_CAUSE        (12),
        RANAP_CAUSE     (13);

        public int value;

        private FieldIndex(int value) {
            this.value = value;
        }
    }
    
    private InetRawUtil() {
    }
    
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
        if (fields.length != FieldIndex.values().length) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return InetRaw.newBuilder()
                .setType(fields[TYPE.value])
                .setCallType(fields[CALL_TYPE.value])
                .setImsi(fields[IMSI.value])
                .setFirstTempImsi(fields[FIRST_TEMP_IMSI.value])
                .setLastTempImsi(fields[LAST_TEMP_IMSI.value])
                .setImei(fields[IMEI.value])
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
