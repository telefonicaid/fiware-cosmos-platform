package es.tid.smartsteps.util;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.nio.charset.Charset;

import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;

/**
 * A CSV printer for CRM-IPM CDRs.
 *
 * @author apv
 */
public class CrmIpmCsvPrinter extends AbstractCsvPrinter<CrmIpm> {

    public CrmIpmCsvPrinter(String delimiter, Charset charset) {
        super(delimiter, charset);
    }

    @Override
    public void print(CrmIpm crmIpm, OutputStream output) throws IOException {
        String delimiter = this.getDelimiter();
        String line = (crmIpm.getAnonymisedMsisdn() + delimiter
                + crmIpm.getAnonymisedBillingPostCode() + delimiter
                + crmIpm.getAcornCode() + delimiter
                + crmIpm.getGender() + delimiter
                + crmIpm.getBillingSystem() + delimiter
                + crmIpm.getMtrcPlSegment() + delimiter
                + crmIpm.getMpnStatus() + delimiter
                + crmIpm.getSpid() + delimiter
                + crmIpm.getActiveStatus() + delimiter
                + crmIpm.getNeedsSegmentation() + delimiter
                + crmIpm.getAge() + delimiter
                + crmIpm.getAgeBand() + delimiter
                + crmIpm.getAnonymisedImsi() + delimiter
                + crmIpm.getAnonymisedImei() + delimiter
                + crmIpm.getImeiTac() + delimiter
                + crmIpm.getDeviceType() + delimiter
                + crmIpm.getDeviceManufacturer() + delimiter
                + crmIpm.getDeviceModelName() + delimiter
                + crmIpm.getEffectiveFromDate());
        Writer outputWriter = new OutputStreamWriter(output, this.getCharset());
        outputWriter.write(line);
        outputWriter.close();
    }
}
