package es.tid.smartsteps.ipm;

import java.nio.charset.Charset;
import java.security.NoSuchAlgorithmException;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.base.util.SHAEncoder;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;
import es.tid.smartsteps.util.*;

/**
 *
 * @author dmicol
 */
public class CrmRawToIpmConverter extends
        RawToIpmConverterSupport<CrmRaw, CrmIpm> {

    private static final int IMEI_LENGTH = 15;
    private static final int IMEI_PREFIX_LENGTH = 8;

    @Override
    protected CrmIpm convert(CrmRaw crmRaw) {
        final String anonymisedMsisdn;
        final String anonymisedBillingPostCode;
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
            throw new IllegalArgumentException("Failed to anonymize data", ex);
        }
        final String imeiTac = (crmRaw.getImei().length() == IMEI_LENGTH) ?
                crmRaw.getImei().substring(0, IMEI_PREFIX_LENGTH) : "";
        return CrmIpm.newBuilder()
                .setAnonymisedMsisdn(anonymisedMsisdn)
                .setAnonymisedBillingPostCode(anonymisedBillingPostCode)
                .setBillingPostCodePrefix("")
                .setAcornCode(crmRaw.getAcornCode())
                .setGender(crmRaw.getGender())
                .setBillingSystem(crmRaw.getBillingSystem())
                .setMtrcPlSegment(crmRaw.getMtrcPlSegment())
                .setMpnStatus(crmRaw.getMpnStatus())
                .setSpid(crmRaw.getSpid())
                .setActiveStatus(crmRaw.getActiveStatus())
                .setNeedsSegmentation(crmRaw.getNeedsSegmentation())
                .setAge(crmRaw.getAge())
                .setAgeBand(crmRaw.getAgeBand())
                .setAnonymisedImsi(anonymisedImsi)
                .setAnonymisedImei(anonymisedImei)
                .setImeiTac(imeiTac)
                .setDeviceType(crmRaw.getDeviceType())
                .setDeviceManufacturer(crmRaw.getDeviceManufacturer())
                .setDeviceModelName(crmRaw.getDeviceModelName())
                .setEffectiveFromDate(crmRaw.getEffectiveFromDate())
                .build();
    }

    @Override
    protected CrmRawCsvParser newParser() {
        return new CrmRawCsvParser(this.getDelimiter(), this.getCharset());
    }

    @Override
    protected CrmIpmCsvPrinter newPrinter() {
        return new CrmIpmCsvPrinter(this.getDelimiter(), this.getCharset());
    }

    public CrmRawToIpmConverter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    public static final class Builder implements RawToIpmConverter.Builder {

        @Override
        public RawToIpmConverter newConverter(String delimiter,
                                              Charset charset) {
            return new CrmRawToIpmConverter(delimiter, charset);
        }
    }
}
