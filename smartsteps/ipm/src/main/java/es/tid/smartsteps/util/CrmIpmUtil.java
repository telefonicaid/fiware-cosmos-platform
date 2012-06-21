package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;

import static es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm.*;

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
                .setBillingPostCodePrefix(billingPostCodePrefix)
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
        if (fields.length != CrmIpm.getDescriptor().getFields().size()) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return CrmIpm.newBuilder()
                .setAnonymisedMsisdn(fields[ANONYMISEDMSISDN_FIELD_NUMBER - 1])
                .setAnonymisedBillingPostCode(
                        fields[ANONYMISEDBILLINGPOSTCODE_FIELD_NUMBER - 1])
                .setAcornCode(fields[ACORNCODE_FIELD_NUMBER - 1])
                .setBillingPostCodePrefix(
                        fields[BILLINGPOSTCODEPREFIX_FIELD_NUMBER - 1])
                .setGender(fields[GENDER_FIELD_NUMBER - 1])
                .setBillingSystem(fields[BILLINGSYSTEM_FIELD_NUMBER - 1])
                .setMtrcPlSegment(fields[MTRCPLSEGMENT_FIELD_NUMBER - 1])
                .setMpnStatus(fields[MPNSTATUS_FIELD_NUMBER - 1])
                .setSpid(fields[SPID_FIELD_NUMBER - 1])
                .setActiveStatus(fields[ACTIVESTATUS_FIELD_NUMBER - 1])
                .setNeedsSegmentation(
                        fields[NEEDSSEGMENTATION_FIELD_NUMBER - 1])
                .setAge(fields[AGE_FIELD_NUMBER - 1])
                .setAgeBand(fields[AGEBAND_FIELD_NUMBER - 1])
                .setAnonymisedImsi(fields[ANONYMISEDIMSI_FIELD_NUMBER - 1])
                .setAnonymisedImei(fields[ANONYMISEDIMEI_FIELD_NUMBER - 1])
                .setImeiTac(fields[IMEITAC_FIELD_NUMBER - 1])
                .setDeviceType(fields[DEVICETYPE_FIELD_NUMBER - 1])
                .setDeviceManufacturer(
                        fields[DEVICEMANUFACTURER_FIELD_NUMBER - 1])
                .setDeviceModelName(fields[DEVICEMODELNAME_FIELD_NUMBER - 1])
                .setEffectiveFromDate(
                        fields[EFFECTIVEFROMDATE_FIELD_NUMBER - 1])
                .build();
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
