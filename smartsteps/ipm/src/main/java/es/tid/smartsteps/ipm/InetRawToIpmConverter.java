package es.tid.smartsteps.ipm;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.base.util.SHAEncoder;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;
import es.tid.smartsteps.util.InetIpmUtil;
import es.tid.smartsteps.util.InetRawUtil;

import java.security.NoSuchAlgorithmException;

/**
 *
 * @author dmicol
 */
public class InetRawToIpmConverter implements RawToIpmConverter {
    public InetRawToIpmConverter() {
    }
    
    @Override
    public String convert(String line) throws ParseException {
        final InetRaw inetRaw = InetRawUtil.parse(line);
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
}
