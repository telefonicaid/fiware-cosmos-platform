package es.tid.cosmos.platform.injection.ipm;

import java.security.NoSuchAlgorithmException;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.platform.injection.ipm.data.generated.CrmProtocol.CrmIpm;
import es.tid.cosmos.platform.injection.ipm.data.generated.CrmProtocol.CrmRaw;
import es.tid.cosmos.platform.injection.ipm.util.CrmIpmUtil;
import es.tid.cosmos.platform.injection.ipm.util.CrmRawUtil;
import es.tid.cosmos.base.util.SHAEncoder;

/**
 *
 * @author dmicol
 */
public class CrmRawToIpmConverter implements RawToItpmConverter {
    public CrmRawToIpmConverter() {
    }
    
    @Override
    public String convert(String line) {
        final CrmRaw crmRaw = CrmRawUtil.parse(line);
        String anonymisedMsisdn;
        String anonymisedBillingPostCode;
        final String anonymisedImsi;
        final String anonymisedImei;
        try {
            anonymisedMsisdn = SHAEncoder.encode(crmRaw.getMsisdn());
            anonymisedBillingPostCode = SHAEncoder.encode(
                    crmRaw.getBillingPostCode());
            anonymisedImsi = SHAEncoder.encode(crmRaw.getImsi());
            anonymisedImei = SHAEncoder.encode(crmRaw.getImei());
        } catch (NoSuchAlgorithmException ex) {
            Logger.get(CrmRawToIpmConverter.class).fatal(ex);
            throw new IllegalArgumentException("Failed to anonimise data", ex);
        }
        final String imeiTac = (crmRaw.getImei().length() == 15) ?
                crmRaw.getImei().substring(0, 8) : "";
        final CrmIpm crmIpm = CrmIpmUtil.create(anonymisedMsisdn,
                anonymisedBillingPostCode, "", crmRaw.getAcornCode(),
                crmRaw.getGender(), crmRaw.getBillingSystem(),
                crmRaw.getMtrcPlSegment(), crmRaw.getMpnStatus(),
                crmRaw.getSpid(), crmRaw.getActiveStatus(),
                crmRaw.getNeedsSegmentation(), crmRaw.getAge(),
                crmRaw.getAgeBand(), anonymisedImsi, anonymisedImei, imeiTac,
                crmRaw.getDeviceType(), crmRaw.getDeviceManufacturer(),
                crmRaw.getDeviceModelName(), crmRaw.getEffectiveFromDate());
        return CrmIpmUtil.toString(crmIpm);
    }
}
