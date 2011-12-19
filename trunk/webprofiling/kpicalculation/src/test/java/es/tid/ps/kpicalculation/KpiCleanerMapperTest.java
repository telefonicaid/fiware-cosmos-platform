package es.tid.ps.kpicalculation;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;

import org.junit.Before;
import org.junit.Test;

import es.tid.ps.kpicalculation.KpiCleanerMapper;
import es.tid.ps.kpicalculation.data.WebLog;
import es.tid.ps.kpicalculation.data.WebLogFactory;

import junit.framework.TestCase;

/**
 * Test case for KpiCleanerMapper
 * 
 * @author javierb
 */
public class KpiCleanerMapperTest extends TestCase {

    private Mapper<LongWritable, Text, NullWritable, WebLog> mapper;
    private MapDriver<LongWritable, Text, NullWritable, WebLog> driver;
    private WebLog outputPage;
    
    
    @Before
    protected void setUp() {
        this.mapper = new KpiCleanerMapper();
        this.driver = new MapDriver<LongWritable, Text, NullWritable, WebLog>(
                this.mapper);
        this.driver.getConfiguration().addResource("kpi-filtering.xml");
        
        this.outputPage = new WebLog();
        this.outputPage.visitorId = "16737b1873ef03ad";
        this.outputPage.protocol = "http";
        this.outputPage.fullUrl = "http://0.0.tid.es/";
        this.outputPage.urlDomain = "0.0.tid.es";
        this.outputPage.urlPath = "/";
        this.outputPage.urlQuery = "null";
        this.outputPage.dateView = "01 12 2010";
        this.outputPage.timeDay = "00:00:01";
        this.outputPage.browser = "-Microsoft-CryptoAPI/6.1";
        this.outputPage.device = "-Microsoft-CryptoAPI/6.1";
        this.outputPage.userAgent = "-Microsoft-CryptoAPI/6.1";
        this.outputPage.operSys = "-Microsoft-CryptoAPI/6.1";
        this.outputPage.method = "GET";
        this.outputPage.status = "304";
    }

    @Test
    public void testAllowedExtension() {
        List<Pair<NullWritable, WebLog>> out = null;
        String input = "16737b1873ef03ad\thttp://www.0.0.tid.es/index.html\t" +
                "1Dec2010000001\t304\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";
       

        try {
            
            out = this.driver.withInput(new LongWritable(0), new Text(input))
                    .run();
        } catch (IOException ioe) {
            fail();
        }
       
        List<Pair<NullWritable, WebLog>> expected = new ArrayList<Pair<NullWritable, WebLog>>();
        expected.add(new Pair<NullWritable, WebLog>(NullWritable.get(), outputPage));

        assertListEquals(expected, out);
    }

    @Test
    public void testForbiddenExtension() {
        List<Pair<NullWritable, WebLog>> out = null;
        String line = "16737b1873ef03ad\thttp://www.tid.co.uk/foto.jpg\t" +
                "1Dec2010000001\t304\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";
        try {
            out = this.driver.withInput(new LongWritable(0), new Text(line))
                    .run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<NullWritable, WebLog>> expected = new ArrayList<Pair<NullWritable, WebLog>>();

        assertListEquals(expected, out);
    }

    @Test
    public void testThirdPartyDomain() {
        List<Pair<NullWritable, WebLog>> out = null;
        String line = "16737b1873ef03ad\thttp://sexsearch.com/asfad\t" +
                "1Dec2010000001\t304\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";

        try {
            out = this.driver.withInput(new LongWritable(0), new Text(line))
                    .run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<NullWritable, WebLog>> expected = new ArrayList<Pair<NullWritable, WebLog>>();
        assertListEquals(expected, out);
    }

    @Test
    public void testPersonalInfoDomain() {
        List<Pair<NullWritable, WebLog>> out = null;
        String line = "16737b1873ef03ad\thttp://pornhub.com/t1/video\t" +
                "1Dec2010000001\t304\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";

        try {
            out = this.driver.withInput(new LongWritable(0), new Text(line))
                    .run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<NullWritable, WebLog>> expected = new ArrayList<Pair<NullWritable, WebLog>>();
        assertListEquals(expected, out);
    }
    
  
}
