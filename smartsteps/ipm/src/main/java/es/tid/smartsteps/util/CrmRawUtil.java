package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;

import static es.tid.smartsteps.util.CrmRawUtil.FieldIndex.*;
/**
 *
 * @author dmicol
 */
public abstract class CrmRawUtil {
    private static final char DELIMITER = '|';

    public static enum FieldIndex {
        MSISDN              (0),
        BILLING_POST_CODE   (1),
        ACORN_CODE          (2),
        GENDER              (3),
        BILLING_SYSTEM      (4),
        MTRC_PL_SEGMENT     (5),
        MPN_STATUS          (6),
        SPID                (7),
        ACTIVE_STATUS       (8),
        NEEDS_SEGMENTATION  (9),
        AGE                 (10),
        AGE_BAND            (11),
        IMSI                (12),
        IMEI                (13),
        DEVICE_TYPE         (14),
        DEVICE_MANUFACTURER (15),
        DEVICE_MODEL_NAME   (16),
        EFFECTIVE_FROM_DATE (17);

        public int value;

        private FieldIndex(int value) {
            this.value = value;
        }
    }
    
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
        if (fields.length != FieldIndex.values().length) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return CrmRaw.newBuilder()
                .setMsisdn(fields[MSISDN.value])
                .setBillingPostCode(fields[BILLING_POST_CODE.value])
                .setAcornCode(fields[ACORN_CODE.value])
                .setGender(fields[GENDER.value])
                .setBillingSystem(fields[BILLING_SYSTEM.value])
                .setMtrcPlSegment(fields[MTRC_PL_SEGMENT.value])
                .setMpnStatus(fields[MPN_STATUS.value])
                .setSpid(fields[SPID.value])
                .setActiveStatus(fields[ACTIVE_STATUS.value])
                .setNeedsSegmentation(fields[NEEDS_SEGMENTATION.value])
                .setAge(fields[AGE.value])
                .setAgeBand(fields[AGE_BAND.value])
                .setImsi(fields[IMSI.value])
                .setImei(fields[IMEI.value])
                .setDeviceType(fields[DEVICE_TYPE.value])
                .setDeviceManufacturer(fields[DEVICE_MANUFACTURER.value])
                .setDeviceModelName(fields[DEVICE_MODEL_NAME.value])
                .setEffectiveFromDate(fields[EFFECTIVE_FROM_DATE.value])
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
