package es.tid.smartsteps.ipm;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;
import java.util.List;

import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmIpm;
import es.tid.smartsteps.ipm.data.generated.CrmProtocol.CrmRaw;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetRaw;
import es.tid.smartsteps.ipm.data.generated.InetProtocol.InetIpm;
import es.tid.smartsteps.util.CrmIpmCsvParser;
import es.tid.smartsteps.util.CrmRawCsvParser;
import es.tid.smartsteps.util.InetIpmCsvParser;
import es.tid.smartsteps.util.InetRawCsvParser;

import static junit.framework.Assert.assertEquals;

/**
 * Unit test for CSV parser classes
 *
 * @author apv
 */
public class CsvParserTest {

    private String separator;
    private Charset charset;

    private CrmRawCsvParser crmRawParser;
    private CrmIpmCsvParser crmIpmParser;
    private InetRawCsvParser inetRawParser;
    private InetIpmCsvParser inetIpmParser;

    @Before
    public void setup() {
        this.separator = "|";
        this.charset = Charset.forName("UTF-8");

        this.crmRawParser = new CrmRawCsvParser(this.separator, this.charset);
        this.crmIpmParser = new CrmIpmCsvParser(this.separator, this.charset);
        this.inetRawParser = new InetRawCsvParser(this.separator, this.charset);
        this.inetIpmParser = new InetIpmCsvParser(this.separator, this.charset);
    }

    @Test
    public void testParseValidCrmRaw() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<CrmRaw> crms = this.crmRawParser.parse(input);
        assertEquals(crms.size(), 1);
    }

    @Test(expected = ParseException.class)
    public void testParseInvalidCrmRaw() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<CrmRaw> crms = this.crmRawParser.parse(input);
        assertEquals(crms.size(), 1);
    }

    @Test
    public void testParseValidCrmIpm() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<CrmIpm> crms = this.crmIpmParser.parse(input);
        assertEquals(crms.size(), 1);
    }

    @Test(expected = ParseException.class)
    public void testParseInvalidCrmIpm() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<CrmRaw> crms = this.crmRawParser.parse(input);
        assertEquals(crms.size(), 1);
    }

    @Test
    public void testParseValidInetRaw() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8|9|10|11|12|13|14";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<InetRaw> crms = this.inetRawParser.parse(input);
        assertEquals(crms.size(), 1);
    }

    @Test(expected = ParseException.class)
    public void testParseInvalidInetRaw() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<InetRaw> crms = this.inetRawParser.parse(input);
        assertEquals(crms.size(), 1);
    }

    @Test
    public void testParseValidInetIpm() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<InetIpm> crms = this.inetIpmParser.parse(input);
        assertEquals(crms.size(), 1);
    }

    @Test(expected = ParseException.class)
    public void testParseInvalidInetIpm() throws IOException, ParseException {
        String cdr = "1|2|3|4|5|6|7|8";
        InputStream input = new ByteArrayInputStream(cdr.getBytes(this.charset));
        List<InetIpm> crms = this.inetIpmParser.parse(input);
        assertEquals(crms.size(), 1);
    }
}
