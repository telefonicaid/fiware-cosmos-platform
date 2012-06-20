package es.tid.smartsteps.ipm;

import java.nio.charset.Charset;
import java.security.NoSuchAlgorithmException;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.base.util.SHAEncoder;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;
import es.tid.smartsteps.util.InetIpmCsvPrinter;
import es.tid.smartsteps.util.InetRawCsvParser;

/**
 *
 * @author dmicol
 * @author apv
 */
public class InetRawToIpmConverter extends
        RawToIpmConverterSupport<InetRaw, InetIpm> {

    private static final int IMEI_LENGTH = 15;
    private static final int IMEI_PREFIX_LENGTH = 8;

    @Override
    protected InetIpm convert(InetRaw inetRaw) {
        String anonymisedImsi;
        String anonymisedImei;
        try {
            anonymisedImsi = SHAEncoder.encode(inetRaw.getImsi());
            anonymisedImei = SHAEncoder.encode(inetRaw.getImei());
        } catch (NoSuchAlgorithmException ex) {
            Logger.get(InetRawToIpmConverter.class).fatal(ex);
            throw new IllegalArgumentException("Failed to anonimise data", ex);
        }
        String imeiTac = (inetRaw.getImei().length() == IMEI_LENGTH) ?
                inetRaw.getImei().substring(0, IMEI_PREFIX_LENGTH) : "";
        return InetIpm.newBuilder()
                .setType(inetRaw.getType())
                .setCallType(inetRaw.getCallType())
                .setAnonymisedImsi(anonymisedImsi)
                .setFirstTempImsi(inetRaw.getFirstTempImsi())
                .setLastTempImsi(inetRaw.getLastTempImsi())
                .setImeiTac(imeiTac)
                .setAnonymisedImei(anonymisedImei)
                .setLacod(inetRaw.getLacod())
                .setCellId(inetRaw.getCellId())
                .setEventDateTime(inetRaw.getEventDateTime())
                .setDtapCause(inetRaw.getDtapCause())
                .setBssmapCause(inetRaw.getBssmapCause())
                .setCcCause(inetRaw.getCcCause())
                .setMmCause(inetRaw.getMmCause())
                .setRanapCause(inetRaw.getRanapCause())
                .build();
    }

    @Override
    protected InetRawCsvParser newParser() {
        return new InetRawCsvParser(this.getDelimiter(), this.getCharset());
    }

    @Override
    protected InetIpmCsvPrinter newPrinter() {
        return new InetIpmCsvPrinter(this.getDelimiter(), this.getCharset());
    }

    public InetRawToIpmConverter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    public static final class Builder implements RawToIpmConverter.Builder {

        @Override
        public RawToIpmConverter newConverter(String delimiter,
                                              Charset charset) {
            return new InetRawToIpmConverter(delimiter, charset);
        }
    }
}
