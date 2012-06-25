package es.tid.smartsteps.ipm;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.Charset;

import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;
import es.tid.smartsteps.util.CrmIpmCsvPrinter;
import es.tid.smartsteps.util.CrmRawCsvPrinter;
import es.tid.smartsteps.util.InetIpmCsvPrinter;
import es.tid.smartsteps.util.InetRawCsvPrinter;

import static junit.framework.Assert.assertEquals;

/**
 * Unit test for CSV printers
 *
 * @author apv
 */
public class CsvPrinterTest {

    private String separator;
    private Charset charset;

    private CrmRawCsvPrinter crmRawPrinter;
    private CrmIpmCsvPrinter crmIpmPrinter;
    private InetRawCsvPrinter inetRawPrinter;
    private InetIpmCsvPrinter inetIpmPrinter;

    @Before
    public void setup() {
        this.separator = "|";
        this.charset = Charset.forName("UTF-8");

        this.crmRawPrinter = new CrmRawCsvPrinter(this.separator, this.charset);
        this.crmIpmPrinter = new CrmIpmCsvPrinter(this.separator, this.charset);
        this.inetRawPrinter = new InetRawCsvPrinter(this.separator, this.charset);
        this.inetIpmPrinter = new InetIpmCsvPrinter(this.separator, this.charset);
    }

    @Test
    public void testPrintCrmRaw() throws IOException {
        CrmRaw obj = CrmRaw.newBuilder()
                .setMsisdn("1")
                .setBillingPostCode("2")
                .setAcornCode("3")
                .setGender("4")
                .setBillingSystem("5")
                .setMtrcPlSegment("6")
                .setMpnStatus("7")
                .setSpid("8")
                .setActiveStatus("9")
                .setNeedsSegmentation("10")
                .setAge("11")
                .setAgeBand("12")
                .setImsi("13")
                .setImei("14")
                .setDeviceType("15")
                .setDeviceManufacturer("16")
                .setDeviceModelName("17")
                .setEffectiveFromDate("18")
                .build();
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        this.crmRawPrinter.print(obj, output);
        String line = new String(output.toByteArray());
        assertEquals(line, "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18");
    }

    @Test
    public void testPrintCrmIpm() throws IOException {
        CrmIpm obj = CrmIpm.newBuilder()
                .setAnonymisedMsisdn("1")
                .setAnonymisedBillingPostCode("2")
                .setBillingPostCodePrefix("3")
                .setAcornCode("4")
                .setGender("5")
                .setBillingSystem("6")
                .setMtrcPlSegment("7")
                .setMpnStatus("8")
                .setSpid("9")
                .setActiveStatus("10")
                .setNeedsSegmentation("11")
                .setAge("12")
                .setAgeBand("13")
                .setAnonymisedImsi("14")
                .setAnonymisedImei("15")
                .setImeiTac("16")
                .setDeviceType("17")
                .setDeviceManufacturer("18")
                .setDeviceModelName("19")
                .setEffectiveFromDate("20")
                .build();
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        this.crmIpmPrinter.print(obj, output);
        String line = new String(output.toByteArray());
        assertEquals(line,
                "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20");
    }

    @Test
    public void testPrintInetRaw() throws IOException {
        InetRaw obj = InetRaw.newBuilder()
                .setType("1")
                .setCallType("2")
                .setImsi("3")
                .setFirstTempImsi("4")
                .setLastTempImsi("5")
                .setImei("6")
                .setLacod("7")
                .setCellId("8")
                .setEventDateTime("9")
                .setDtapCause("10")
                .setBssmapCause("11")
                .setCcCause("12")
                .setMmCause("13")
                .setRanapCause("14")
                .build();
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        this.inetRawPrinter.print(obj, output);
        String line = new String(output.toByteArray());
        assertEquals(line,
                "1|2|3|4|5|6|7|8|9|10|11|12|13|14");
    }

    @Test
    public void testPrintInetIpm() throws IOException {
        InetIpm obj = InetIpm.newBuilder()
                .setType("1")
                .setCallType("2")
                .setAnonymisedImsi("3")
                .setFirstTempImsi("4")
                .setLastTempImsi("5")
                .setImeiTac("6")
                .setAnonymisedImei("7")
                .setLacod("8")
                .setCellId("9")
                .setEventDateTime("10")
                .setDtapCause("11")
                .setBssmapCause("12")
                .setCcCause("13")
                .setMmCause("14")
                .setRanapCause("15")
                .build();
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        this.inetIpmPrinter.print(obj, output);
        String line = new String(output.toByteArray());
        assertEquals(line,
                "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15");
    }
}
