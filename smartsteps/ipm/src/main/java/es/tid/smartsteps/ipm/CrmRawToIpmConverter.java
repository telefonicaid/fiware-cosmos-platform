package es.tid.smartsteps.ipm;

import java.io.ByteArrayInputStream;
import java.nio.charset.Charset;
import java.security.NoSuchAlgorithmException;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.base.util.SHAEncoder;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;
import es.tid.smartsteps.util.CrmIpmUtil;
import es.tid.smartsteps.util.CrmRawCsvParser;

/**
 *
 * @author dmicol
 */
public class CrmRawToIpmConverter extends AbstractRawToIpmConverter {

    public CrmRawToIpmConverter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public String convert(String line) throws ParseException {
        final CrmRawCsvParser csvParser = new CrmRawCsvParser(
                this.getDelimiter(), this.getCharset());
        final CrmRaw crmRaw = csvParser.parse(
                new ByteArrayInputStream(line.getBytes(this.getCharset())));
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

    public static final class Builder implements RawToIpmConverter.Builder {

        @Override
        public RawToIpmConverter newConverter(String delimiter,
                                              Charset charset) {
            return new CrmRawToIpmConverter(delimiter, charset);
        }
    }
}
