package es.tid.smartsteps.util;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;

/**
 * A CSV printer for INET-IPM CDRs.
 *
 * @author apv
 */
public class InetIpmCsvPrinter extends AbstractCsvPrinter<InetIpm> {

    public InetIpmCsvPrinter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public void print(InetIpm inetIpm, OutputStream output) throws IOException {
        String delimiter = this.getDelimiter();
        String line = (inetIpm.getType() + delimiter
                + inetIpm.getCallType() + delimiter
                + inetIpm.getAnonymisedImsi() + delimiter
                + inetIpm.getFirstTempImsi() + delimiter
                + inetIpm.getLastTempImsi() + delimiter
                + inetIpm.getImeiTac() + delimiter
                + inetIpm.getAnonymisedImei() + delimiter
                + inetIpm.getLacod() + delimiter
                + inetIpm.getCellId() + delimiter
                + inetIpm.getEventDateTime() + delimiter
                + inetIpm.getDtapCause() + delimiter
                + inetIpm.getBssmapCause() + delimiter
                + inetIpm.getCcCause() + delimiter
                + inetIpm.getMmCause() + delimiter
                + inetIpm.getRanapCause());
        Writer outputWriter = new OutputStreamWriter(output, this.getCharset());
        outputWriter.write(line);
        outputWriter.close();
    }
}
