package es.tid.smartsteps.util;

import es.tid.smartsteps.ipm.ParseException;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;

import static es.tid.smartsteps.util.CrmIpmUtil.FieldIndex.*;

/**
 *
 * @author dmicol
 */
public abstract class CrmIpmUtil {
    private static final char DELIMITER = '|';

    public static enum FieldIndex {
        ANONYMISED_MSISDN               (0),
        ANONYMISED_BILLING_POST_CODE    (1),
        BILLING_POST_CODE_PREFIX        (2),
        ACORN_CODE                      (3),
        GENDER                          (4),
        BILLING_SYSTEM                  (5),
        MTRC_PL_SEGMENT                 (6),
        MPN_STATUS                      (7),
        SPID                            (8),
        ACTIVE_STATUS                   (9),
        NEEDS_SEGMENTATION              (10),
        AGE                             (11),
        AGE_BAND                        (12),
        ANONYMISED_IMSI                 (13),
        ANONYMISED_IMEI                 (14),
        IMEI_TAC                        (15),
        DEVICE_TYPE                     (16),
        DEVICE_MANUFACTURER             (17),
        DEVICE_MODEL_NAME               (18),
        EFFECTIVE_FROM_NAME             (19);

        public final int value;

        private FieldIndex(int value) {
            this.value = value;
        }
    }

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
        if (fields.length != FieldIndex.values().length) {
            throw new ParseException(String.format(
                    "cannot parse input line %s: invalid format", line));
        }
        return CrmIpm.newBuilder()
                .setAnonymisedMsisdn(fields[ANONYMISED_MSISDN.value])
                .setAnonymisedBillingPostCode(fields[ANONYMISED_BILLING_POST_CODE.value])
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
                .setAnonymisedImsi(fields[ANONYMISED_IMSI.value])
                .setAnonymisedImei(fields[ANONYMISED_IMEI.value])
                .setImeiTac(fields[IMEI_TAC.value])
                .setDeviceType(fields[DEVICE_TYPE.value])
                .setDeviceManufacturer(fields[DEVICE_MANUFACTURER.value])
                .setDeviceModelName(fields[DEVICE_MODEL_NAME.value])
                .setEffectiveFromDate(fields[EFFECTIVE_FROM_NAME.value])
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
