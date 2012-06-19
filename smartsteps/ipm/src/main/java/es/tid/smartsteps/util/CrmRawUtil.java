package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;

import static es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw.*;
/**
 *
 * @author dmicol
 */
public abstract class CrmRawUtil {
    private static final char DELIMITER = '|';

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
    
    public static CrmRaw parse(String line) throws ParseException {
        String[] fields = line.split("\\" + DELIMITER);
        if (fields.length != CrmRaw.getDescriptor().getFields().size()) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return CrmRaw.newBuilder()
                .setMsisdn(fields[MSISDN_FIELD_NUMBER - 1])
                .setBillingPostCode(fields[BILLINGPOSTCODE_FIELD_NUMBER - 1])
                .setAcornCode(fields[ACORNCODE_FIELD_NUMBER - 1])
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
                .setImsi(fields[IMSI_FIELD_NUMBER - 1])
                .setImei(fields[IMEI_FIELD_NUMBER - 1])
                .setDeviceType(fields[DEVICETYPE_FIELD_NUMBER - 1])
                .setDeviceManufacturer(
                        fields[DEVICEMANUFACTURER_FIELD_NUMBER - 1])
                .setDeviceModelName(fields[DEVICEMODELNAME_FIELD_NUMBER - 1])
                .setEffectiveFromDate(
                        fields[EFFECTIVEFROMDATE_FIELD_NUMBER - 1])
                .build();
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
