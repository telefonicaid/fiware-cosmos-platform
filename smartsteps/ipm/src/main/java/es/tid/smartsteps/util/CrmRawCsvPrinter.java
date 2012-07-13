package es.tid.smartsteps.util;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;

/**
 * A CSV printer for CRM-RAW CDRs.
 *
 * @author apv
 */
public class CrmRawCsvPrinter extends AbstractCsvPrinter<CrmRaw> {

    public CrmRawCsvPrinter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public void print(CrmRaw crmRaw, OutputStream output) throws IOException {
        String delimiter = this.getDelimiter();
        String line = (crmRaw.getMsisdn() + delimiter
                + crmRaw.getBillingPostCode() + delimiter
                + crmRaw.getAcornCode() + delimiter
                + crmRaw.getGender() + delimiter
                + crmRaw.getBillingSystem() + delimiter
                + crmRaw.getMtrcPlSegment() + delimiter
                + crmRaw.getMpnStatus() + delimiter
                + crmRaw.getSpid() + delimiter
                + crmRaw.getActiveStatus() + delimiter
                + crmRaw.getNeedsSegmentation() + delimiter
                + crmRaw.getAge() + delimiter
                + crmRaw.getAgeBand() + delimiter
                + crmRaw.getImsi() + delimiter
                + crmRaw.getImei() + delimiter
                + crmRaw.getDeviceType() + delimiter
                + crmRaw.getDeviceManufacturer() + delimiter
                + crmRaw.getDeviceModelName() + delimiter
                + crmRaw.getEffectiveFromDate());
        Writer outputWriter = new OutputStreamWriter(output, this.getCharset());
        outputWriter.write(line);
        outputWriter.close();
    }
}
