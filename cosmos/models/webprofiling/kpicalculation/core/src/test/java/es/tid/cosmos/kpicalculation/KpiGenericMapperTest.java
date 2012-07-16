package es.tid.cosmos.kpicalculation;

import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.base.mapreduce.CompositeKey;
import es.tid.cosmos.base.mapreduce.SingleKey;
import es.tid.cosmos.kpicalculation.generated.data.KpiCalculationProtocol.WebProfilingLog;

/**
 * Test case for KpiCleanerMapper
 *
 * @author javierb
 */
public class KpiGenericMapperTest {
    private Mapper<NullWritable, ProtobufWritable<WebProfilingLog>,
                   CompositeKey, IntWritable> mapper;
    private MapDriver<NullWritable, ProtobufWritable<WebProfilingLog>,
                      CompositeKey, IntWritable> driver;
    private ProtobufWritable<WebProfilingLog> inputLog;

    @Before
    public void setUp() {
        this.mapper = new KpiGenericMapper();
        this.driver = new MapDriver<NullWritable,
                                    ProtobufWritable<WebProfilingLog>,
                                    CompositeKey, IntWritable>(this.mapper);
        this.driver.getConfiguration().setStrings("kpi.aggregation.fields",
                                                  "protocol,device,date");
        this.inputLog = ProtobufWritable.newInstance(WebProfilingLog.class);
        this.inputLog.set(WebProfilingLog.newBuilder()
                .setVisitorId("16737b1873ef03ad")
                .setProtocol("http")
                .setFullUrl("http://tid.es/")
                .setUrlDomain("tid.es")
                .setUrlPath("/")
                .setUrlQuery("null")
                .setDate("02\t11\t2012")
                .setMimeType("application/pkix-crl")
                .setBrowser("-Microsoft-CryptoAPI/6.1")
                .setDevice("-Microsoft-CryptoAPI/6.1")
                .setUserAgent("-Microsoft-CryptoAPI/6.1")
                .setOperSys("-Microsoft-CryptoAPI/6.1")
                .setMethod("GET")
                .setStatus("304")
                .build());
    }

    @Test
    public void testKpiCounter() throws Exception {
        List<Pair<CompositeKey, IntWritable>> out;

        out = this.driver.withInput(NullWritable.get(), this.inputLog).run();

        CompositeKey key = new SingleKey();
        key.set(0, "http\t-Microsoft-CryptoAPI/6.1\t02\t11\t2012");

        List<Pair<CompositeKey, IntWritable>> expected =
                new ArrayList<Pair<CompositeKey, IntWritable>>();
        expected.add(new Pair<CompositeKey, IntWritable>(key,
                new IntWritable(1)));

        assertListEquals(expected, out);
    }

    @Test
    public void testKpiCounterGrouped() throws Exception {
        List<Pair<CompositeKey, IntWritable>> out;
        this.driver.getConfiguration().setStrings("kpi.aggregation.group",
                "visitorId");
        CompositeKey key = new BinaryKey();
        key.set(0, "http\t-Microsoft-CryptoAPI/6.1\t02\t11\t2012");
        key.set(1, "16737b1873ef03ad");

        out = this.driver.withInput(NullWritable.get(), this.inputLog).run();

        List<Pair<CompositeKey, IntWritable>> expected =
                new ArrayList<Pair<CompositeKey, IntWritable>>();
        expected.add(new Pair<CompositeKey, IntWritable>(key,
                new IntWritable(1)));

        assertListEquals(expected, out);
    }
}
