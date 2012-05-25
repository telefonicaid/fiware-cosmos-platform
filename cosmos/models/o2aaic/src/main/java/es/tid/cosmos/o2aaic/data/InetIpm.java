package es.tid.cosmos.o2aaic.data;

/**
 *
 * @author dmicol
 */
public class InetIpm {
    private final String type;
    private final String callType;
    private final String anonymisedImsi;
    private final String firstTempImsi;
    private final String lastTempImsi;
    private final String imeiTac;
    private final String anonymisedImei;
    private final String lacod;
    private final String cellId;
    private final String eventDateTime;
    private final String dtapCause;
    private final String bssmapCause;
    private final String ccCause;
    private final String mmCause;
    private final String ranapCause;

    public InetIpm(String type, String callType, String anonymisedImsi,
                   String firstTempImsi, String lastTempImsi, String imeiTac,
                   String anonymisedImei, String lacod, String cellId,
                   String eventDateTime, String dtapCause, String bssmapCause,
                   String ccCause, String mmCause, String ranapCause) {
        this.type = type;
        this.callType = callType;
        this.anonymisedImsi = anonymisedImsi;
        this.firstTempImsi = firstTempImsi;
        this.lastTempImsi = lastTempImsi;
        this.imeiTac = imeiTac;
        this.anonymisedImei = anonymisedImei;
        this.lacod = lacod;
        this.cellId = cellId;
        this.eventDateTime = eventDateTime;
        this.dtapCause = dtapCause;
        this.bssmapCause = bssmapCause;
        this.ccCause = ccCause;
        this.mmCause = mmCause;
        this.ranapCause = ranapCause;
    }
    
    public String getBssmapCause() {
        return this.bssmapCause;
    }

    public String getCallType() {
        return this.callType;
    }

    public String getCcCause() {
        return this.ccCause;
    }

    public String getCellId() {
        return this.cellId;
    }

    public String getDtapCause() {
        return this.dtapCause;
    }

    public String getEventDateTime() {
        return this.eventDateTime;
    }

    public String getFirstTempImsi() {
        return this.firstTempImsi;
    }

    public String getImeiTac() {
        return this.imeiTac;
    }
    
    public String getAnonymisedImei() {
        return this.anonymisedImei;
    }

    public String getAnonymisedImsi() {
        return this.anonymisedImsi;
    }

    public String getLacod() {
        return this.lacod;
    }

    public String getLastTempImsi() {
        return this.lastTempImsi;
    }

    public String getMmCause() {
        return this.mmCause;
    }

    public String getRanapCause() {
        return this.ranapCause;
    }

    public String getType() {
        return this.type;
    }
}
