package es.tid.cosmos.platform.injection.ipm.util;

import es.tid.cosmos.platform.injection.ipm.data.generated.CrmProtocol.CrmRaw;

/**
 *
 * @author dmicol
 */
public abstract class CrmRawUtil {
    private static final String DELIMITER = "\\|";
    
    private CrmRawUtil() {
    }
    
    public static CrmRaw create(String msisdn, String billingPostCode,
            String acornCode, String gender, String billingSystem,
            String mtrcPlSegment, String mpnStatus, String spid,
            String activeStatus, String needsSegmentation, String age,
            String ageBand, String imsi, String imei, String deviceType,
            String deviceManufacturer, String deviceModelName,
            String effectiveFromDate) {
        return CrmRaw.newBuilder()
                .setMsisdn(msisdn)
                .setBillingPostCode(billingPostCode)
                .setAcornCode(acornCode)
                .setGender(gender)
                .setBillingSystem(billingSystem)
                .setMtrcPlSegment(mtrcPlSegment)
                .setMpnStatus(mpnStatus)
                .setSpid(spid)
                .setActiveStatus(activeStatus)
                .setNeedsSegmentation(needsSegmentation)
                .setAge(age)
                .setAgeBand(ageBand)
                .setImsi(imsi)
                .setImei(imei)
                .setDeviceType(deviceType)
                .setDeviceManufacturer(deviceManufacturer)
                .setDeviceModelName(deviceModelName)
                .setEffectiveFromDate(effectiveFromDate)
                .build();
    }
    
    public static CrmRaw parse(String line) {
        String[] fields = line.split(DELIMITER);
        return create(fields[0], fields[1], fields[2], fields[3], fields[4],
                fields[5], fields[6], fields[7], fields[8], fields[9],
                fields[10], fields[11], fields[12], fields[13], fields[14],
                fields[15], fields[16], fields[17]);
    }
    
    public static String toString(CrmRaw crmRaw) {
        return (crmRaw.getMsisdn() + DELIMITER
                + crmRaw.getBillingPostCode() + DELIMITER
                + crmRaw.getAcornCode() + DELIMITER
                + crmRaw.getGender() + DELIMITER
                + crmRaw.getBillingSystem() + DELIMITER
                + crmRaw.getMtrcPlSegment() + DELIMITER
                + crmRaw.getMpnStatus() + DELIMITER
                + crmRaw.getSpid() + DELIMITER
                + crmRaw.getActiveStatus() + DELIMITER
                + crmRaw.getNeedsSegmentation() + DELIMITER
                + crmRaw.getAge() + DELIMITER
                + crmRaw.getAgeBand() + DELIMITER
                + crmRaw.getImsi() + DELIMITER
                + crmRaw.getImei() + DELIMITER
                + crmRaw.getDeviceType() + DELIMITER
                + crmRaw.getDeviceManufacturer() + DELIMITER
                + crmRaw.getDeviceModelName() + DELIMITER
                + crmRaw.getEffectiveFromDate());
    }
}
