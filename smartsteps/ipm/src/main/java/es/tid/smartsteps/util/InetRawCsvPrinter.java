package es.tid.smartsteps.util;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;

/**
 * A CSV printer for INET-RAW CDRs.
 *
 * @author apv
 */
public class InetRawCsvPrinter extends AbstractCsvPrinter<InetRaw> {

    public InetRawCsvPrinter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public void print(InetRaw inetRaw, OutputStream output) throws IOException {
        String delimiter = this.getDelimiter();
        String line = (inetRaw.getType() + delimiter
                + inetRaw.getCallType() + delimiter
                + inetRaw.getImsi() + delimiter
                + inetRaw.getFirstTempImsi() + delimiter
                + inetRaw.getLastTempImsi() + delimiter
                + inetRaw.getImei() + delimiter
                + inetRaw.getLacod() + delimiter
                + inetRaw.getCellId() + delimiter
                + inetRaw.getEventDateTime() + delimiter
                + inetRaw.getDtapCause() + delimiter
                + inetRaw.getBssmapCause() + delimiter
                + inetRaw.getCcCause() + delimiter
                + inetRaw.getMmCause() + delimiter
                + inetRaw.getRanapCause());
        Writer outputWriter = new OutputStreamWriter(output, this.getCharset());
        outputWriter.write(line);
        outputWriter.close();
    }
}
