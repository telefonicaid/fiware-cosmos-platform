package es.tid.cosmos.o2aaic.util;

import es.tid.cosmos.o2aaic.data.generated.InetProtocol.InetRaw;

/**
 *
 * @author dmicol
 */
public abstract class InetRawUtil {
    private static final String DELIMITER = "\\|";
    
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
    
    public static InetRaw parse(String line) {
        String[] fields = line.split(DELIMITER);
        return create(fields[0], fields[1], fields[2], fields[3], fields[4],
                fields[5], fields[6], fields[7], fields[8], fields[9],
                fields[10], fields[11], fields[12], fields[13]);
    }
    
    public static String toString(InetRaw InetRaw) {
        return (InetRaw.getType() + DELIMITER + InetRaw.getCallType()
                + DELIMITER + InetRaw.getImsi() + DELIMITER
                + InetRaw.getFirstTempImsi() + DELIMITER
                + InetRaw.getLastTempImsi() + DELIMITER + InetRaw.getImei()
                + DELIMITER + InetRaw.getLacod() + DELIMITER
                + InetRaw.getCellId() + DELIMITER + InetRaw.getEventDateTime()
                + DELIMITER + InetRaw.getDtapCause() + DELIMITER
                + InetRaw.getBssmapCause() + DELIMITER + InetRaw.getCcCause()
                + DELIMITER + InetRaw.getMmCause() + DELIMITER
                + InetRaw.getRanapCause());
    }
}
