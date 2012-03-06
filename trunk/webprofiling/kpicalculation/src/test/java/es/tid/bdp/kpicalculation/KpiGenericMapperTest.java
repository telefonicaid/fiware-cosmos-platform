package es.tid.bdp.kpicalculation;

import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;

import org.junit.Before;
import org.junit.Test;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.base.mapreduce.CompositeKey;
import es.tid.ps.base.mapreduce.SingleKey;
import es.tid.ps.kpicalculation.KpiGenericMapper;
import es.tid.ps.kpicalculation.data.KpiCalculationProtocol.WebProfilingLog;

import junit.framework.TestCase;

/**
 * Test case for KpiCleanerMapper
 * 
 * @author javierb
 */
public class KpiGenericMapperTest extends TestCase {

    private Mapper<LongWritable, ProtobufWritable<WebProfilingLog>, CompositeKey, IntWritable> mapper;
    private MapDriver<LongWritable, ProtobufWritable<WebProfilingLog>, CompositeKey, IntWritable> driver;
    private ProtobufWritable<WebProfilingLog> inputLog;
    private WebProfilingLog.Builder builder;

    @Before
    protected void setUp() {
        this.mapper = new KpiGenericMapper();
        this.driver = new MapDriver<LongWritable, ProtobufWritable<WebProfilingLog>, CompositeKey, IntWritable>(
                this.mapper);
        driver.getConfiguration().setStrings("kpi.aggregation.fields",
                "protocol,device,date");

        this.builder = WebProfilingLog.newBuilder();
        this.builder.setVisitorId("16737b1873ef03ad");
        this.builder.setProtocol("http");
        this.builder.setFullUrl("http://tid.es/");
        this.builder.setUrlDomain("tid.es");
        this.builder.setUrlPath("/");
        this.builder.setUrlQuery("null");
        this.builder.setDate("02\t11\t2012");
        this.builder.setMimeType("application/pkix-crl");
        this.builder.setBrowser("-Microsoft-CryptoAPI/6.1");
        this.builder.setDevice("-Microsoft-CryptoAPI/6.1");
        this.builder.setUserAgent("-Microsoft-CryptoAPI/6.1");
        this.builder.setOperSys("-Microsoft-CryptoAPI/6.1");
        this.builder.setMethod("GET");
        this.builder.setStatus("304");

        this.inputLog = ProtobufWritable.newInstance(WebProfilingLog.class);
        this.inputLog.set(this.builder.build());
    }

    @Test
    public void testKpiCounter() throws Exception {
        List<Pair<CompositeKey, IntWritable>> out = null;

        out = this.driver.withInput(new LongWritable(1), this.inputLog).run();

        CompositeKey key = new SingleKey();
        key.set(0, "http\t-Microsoft-CryptoAPI/6.1\t02\t11\t2012");

        List<Pair<CompositeKey, IntWritable>> expected = new ArrayList<Pair<CompositeKey, IntWritable>>();
        expected.add(new Pair<CompositeKey, IntWritable>(key,
                new IntWritable(1)));

        assertListEquals(expected, out);
    }

    @Test
    public void testKpiCounterGrouped() throws Exception {
        List<Pair<CompositeKey, IntWritable>> out = null;
        driver.getConfiguration().setStrings("kpi.aggregation.group",
                "visitorId");
        CompositeKey key = new BinaryKey();
        key.set(0, "http\t-Microsoft-CryptoAPI/6.1\t02\t11\t2012");
        key.set(1, "16737b1873ef03ad");

        out = this.driver.withInput(new LongWritable(1), this.inputLog).run();

        List<Pair<CompositeKey, IntWritable>> expected = new ArrayList<Pair<CompositeKey, IntWritable>>();
        expected.add(new Pair<CompositeKey, IntWritable>(key,
                new IntWritable(1)));

        assertListEquals(expected, out);
    }
}
