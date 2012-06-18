package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;

/**
 *
 * @author dmicol
 */
public abstract class CrmIpmUtil {
    private static final char DELIMITER = '|';
    
    private CrmIpmUtil() {
    }
    
    public static CrmIpm create(String anonymisedMsisdn,
            String anonymisedBillingPostCode, String billingPostCodePrefix,
            String acornCode, String gender, String billingSystem,
            String mtrcPlSegment, String mpnStatus, String spid,
            String activeStatus, String needsSegmentation, String age,
            String ageBand, String anonymisedImsi, String anonymisedImei,
            String imeiTac, String deviceType, String deviceManufacturer,
            String deviceModelName, String effectiveFromDate) {
        return CrmIpm.newBuilder()
                .setAnonymisedMsisdn(anonymisedMsisdn)
                .setAnonymisedBillingPostCode(anonymisedBillingPostCode)
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
                .setAnonymisedImsi(anonymisedImsi)
                .setAnonymisedImei(anonymisedImei)
                .setImeiTac(imeiTac)
                .setDeviceType(deviceType)
                .setDeviceManufacturer(deviceManufacturer)
                .setDeviceModelName(deviceModelName)
                .setEffectiveFromDate(effectiveFromDate)
                .build();
    }
    
    public static CrmIpm parse(String line) throws ParseException {
        String[] fields = line.split("\\" + DELIMITER);
        if (fields.length != 20) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return create(fields[0], fields[1], fields[2], fields[3], fields[4],
                fields[5], fields[6], fields[7], fields[8], fields[9],
                fields[10], fields[11], fields[12], fields[13], fields[14],
                fields[15], fields[16], fields[17], fields[18], fields[19]);
    }
    
    public static String toString(CrmIpm crmInet) {
        return (crmInet.getAnonymisedMsisdn() + DELIMITER
                + crmInet.getAnonymisedBillingPostCode() + DELIMITER
                + crmInet.getAcornCode() + DELIMITER
                + crmInet.getGender() + DELIMITER
                + crmInet.getBillingSystem() + DELIMITER
                + crmInet.getMtrcPlSegment() + DELIMITER
                + crmInet.getMpnStatus() + DELIMITER
                + crmInet.getSpid() + DELIMITER
                + crmInet.getActiveStatus() + DELIMITER
                + crmInet.getNeedsSegmentation() + DELIMITER
                + crmInet.getAge() + DELIMITER
                + crmInet.getAgeBand() + DELIMITER
                + crmInet.getAnonymisedImsi() + DELIMITER
                + crmInet.getAnonymisedImei() + DELIMITER
                + crmInet.getImeiTac() + DELIMITER
                + crmInet.getDeviceType() + DELIMITER
                + crmInet.getDeviceManufacturer() + DELIMITER
                + crmInet.getDeviceModelName() + DELIMITER
                + crmInet.getEffectiveFromDate());
    }
}
