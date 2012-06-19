package es.tid.smartsteps.ipm;

import java.io.ByteArrayInputStream;
import java.nio.charset.Charset;
import java.security.NoSuchAlgorithmException;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.base.util.SHAEncoder;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;
import es.tid.smartsteps.util.InetIpmUtil;
import es.tid.smartsteps.util.InetRawCsvParser;

/**
 *
 * @author dmicol
 */
public class InetRawToIpmConverter extends AbstractRawToIpmConverter {

    public InetRawToIpmConverter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public String convert(String line) throws ParseException {
        InetRawCsvParser csvParser = new InetRawCsvParser(this.getDelimiter()
                , this.getCharset());
        final InetRaw inetRaw = csvParser.parse(new ByteArrayInputStream(line
                .getBytes(this.getCharset())));
        final String anonymisedImsi;
        final String anonymisedImei;
        try {
            anonymisedImsi = SHAEncoder.encode(inetRaw.getImsi());
            anonymisedImei = SHAEncoder.encode(inetRaw.getImei());
        } catch (NoSuchAlgorithmException ex) {
            Logger.get(InetRawToIpmConverter.class).fatal(ex);
            throw new IllegalArgumentException("Failed to anonimise data", ex);
        }
        final String imeiTac = (inetRaw.getImei().length() == 15) ?
                inetRaw.getImei().substring(0, 8) : "";
        final InetIpm inetIpm = InetIpmUtil.create(inetRaw.getType(),
                inetRaw.getCallType(), anonymisedImsi,
                inetRaw.getFirstTempImsi(), inetRaw.getLastTempImsi(),
                imeiTac, anonymisedImei, inetRaw.getLacod(),
                inetRaw.getCellId(), inetRaw.getEventDateTime(),
                inetRaw.getDtapCause(), inetRaw.getBssmapCause(),
                inetRaw.getCcCause(), inetRaw.getMmCause(),
                inetRaw.getRanapCause());
        return InetIpmUtil.toString(inetIpm);
    }

    public static final class Builder implements RawToIpmConverter.Builder {

        @Override
        public RawToIpmConverter newConverter(String delimiter,
                                              Charset charset) {
            return new InetRawToIpmConverter(delimiter, charset);
        }
    }
}
