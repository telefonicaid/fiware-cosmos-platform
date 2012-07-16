package es.tid.cosmos.kpicalculation;

import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;
import es.tid.cosmos.kpicalculation.generated.data.KpiCalculationProtocol;
import es.tid.cosmos.kpicalculation.generated.data.KpiCalculationProtocol.WebProfilingLog;

/**
 * Test case for KpiCleanerMapper
 *
 * @author javierb
 */
public class KpiCleanerMapperTest {
    private KpiCleanerMapper mapper;
    private MapDriver<LongWritable, Text, NullWritable,
                      ProtobufWritable<WebProfilingLog>> driver;
    private ProtobufWritable<WebProfilingLog> outputPage;
    private WebProfilingLog.Builder builder;

    @Before
    public void setUp() {

        this.mapper = new KpiCleanerMapper();
        this.driver = new MapDriver<LongWritable, Text, NullWritable,
                ProtobufWritable<KpiCalculationProtocol.WebProfilingLog>>(
                this.mapper);
        this.driver.getConfiguration().addResource("kpi-filtering.xml");

        this.builder = WebProfilingLog.newBuilder();
        this.builder.setVisitorId("16737b1873ef03ad");
        this.builder.setProtocol("http");
        this.builder.setFullUrl("http://www.0.0.tid.es/index.html");
        this.builder.setUrlDomain("tid.es");
        this.builder.setUrlPath("/index.html");
        this.builder.setUrlQuery("null");
        this.builder.setDate("2010-12-01");
        this.builder.setMimeType("application/pkix-crl");
        this.builder.setBrowser("-Microsoft-CryptoAPI/6.1");
        this.builder.setDevice("-Microsoft-CryptoAPI/6.1");
        this.builder.setUserAgent("-Microsoft-CryptoAPI/6.1");
        this.builder.setOperSys("-Microsoft-CryptoAPI/6.1");
        this.builder.setMethod("GET");
        this.builder.setStatus("305");

        this.outputPage = ProtobufWritable.newInstance(WebProfilingLog.class);
        this.outputPage.setConverter(WebProfilingLog.class);
        this.outputPage.set(this.builder.build());

    }

    @Test
    public void testAllowedExtension() throws Exception {
        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> out = null;
        String input = "16737b1873ef03ad\thttp://www.0.0.tid.es/index.html\t"
                + "1Dec2010000001\t305\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";

        out = this.driver.withInput(new LongWritable(0), new Text(input)).run();

        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> expected =
                new ArrayList<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>>();
        expected.add(new Pair<NullWritable, ProtobufWritable<WebProfilingLog>>(
                NullWritable.get(), this.outputPage));

        assertListEquals(expected, out);
    }

    @Test
    public void testForbiddenExtension() throws Exception {
        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> out = null;
        String line = "16737b1873ef03ad\thttp://www.tid.co.uk/foto.jpg\t"
                + "1Dec2010000001\t304\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";

        out = this.driver.withInput(new LongWritable(0), new Text(line)).run();

        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> expected =
                new ArrayList<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>>();

        assertEquals(
                1,
                this.driver
                        .getCounters()
                        .findCounter(
                                KpiCalculationCounter.LINE_FILTERED_EXTENSION)
                        .getValue());
        assertListEquals(expected, out);
    }

    @Test
    public void testThirdPartyDomain() throws Exception {
        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> out = null;
        String line = "16737b1873ef03ad\thttp://sexsearch.com/asfad\t"
                + "1Dec2010000001\t304\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";

        out = this.driver.withInput(new LongWritable(0), new Text(line)).run();

        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> expected =
                new ArrayList<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>>();

        assertEquals(
                1,
                this.driver
                        .getCounters()
                        .findCounter(
                                KpiCalculationCounter.LINE_FILTERED_3RDPARTY)
                        .getValue());
        assertListEquals(expected, out);
    }

    @Test
    public void testPersonalInfoDomain() throws Exception {
        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> out = null;
        String line = "16737b1873ef03ad\thttp://pornhub.com/t1/video\t"
                + "1Dec2010000001\t304\tapplication/pkix-crl\t-Microsoft-CryptoAPI/6.1\tGET";

        out = this.driver.withInput(new LongWritable(0), new Text(line)).run();

        List<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>> expected =
                new ArrayList<Pair<NullWritable, ProtobufWritable<WebProfilingLog>>>();

        assertEquals(
                1,
                this.driver
                        .getCounters()
                        .findCounter(
                                KpiCalculationCounter.LINE_FILTERED_PERSONAL_INFO)
                        .getValue());
        assertListEquals(expected, out);
    }
}
