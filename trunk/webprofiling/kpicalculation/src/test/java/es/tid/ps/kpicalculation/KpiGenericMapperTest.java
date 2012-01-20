package es.tid.ps.kpicalculation;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.apache.hadoop.io.IntWritable;
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
import es.tid.ps.kpicalculation.data.WebLogType;

import junit.framework.TestCase;

/**
 * Test case for KpiCleanerMapper
 * 
 * @author javierb
 */
public class KpiGenericMapperTest extends TestCase {

    private Mapper<LongWritable, Text, WebLog, IntWritable> mapper;
    private MapDriver<LongWritable, Text, WebLog, IntWritable> driver;

    @Before
    protected void setUp() {
        this.mapper = new KpiGenericMapper();
        this.driver = new MapDriver<LongWritable, Text, WebLog, IntWritable>(
                this.mapper);
        driver.getConfiguration().setStrings("kpi.aggregation.fields",
                "protocol,device,date");
    }

    @Test
    public void testKpiCounter() {
        List<Pair<WebLog, IntWritable>> out = null;
       
        driver.getConfiguration().setStrings("kpi.aggregation.type",
                WebLogType.WEB_LOG_COUNTER.toString());
        Text input = new Text(
                "16737b1873ef03ad\thttp\thttp://tid.es/\ttid.es\t"
                        + "/\tnull\t09\t12\t2010\t00\t00\t-Microsoft-CryptoAPI/6.1\t"
                        + "-Microsoft-CryptoAPI/6.1\t-Microsoft-CryptoAPI/6.1\t-Microsoft-CryptoAPI/6.1\t"
                        + "GET\t304");

       try {

            out = this.driver.withInput(new LongWritable(0), input).run();
        } catch (IOException ioe) {
            fail();
        }
                
        WebLog key = WebLogFactory.getWebLog(null, null,
                WebLogType.WEB_LOG_COUNTER);
        key.mainKey = "http\t-Microsoft-CryptoAPI/6.1\t09\t12\t2010";

        List<Pair<WebLog, IntWritable>> expected = new ArrayList<Pair<WebLog, IntWritable>>();
        expected.add(new Pair<WebLog, IntWritable>(key, new IntWritable(1)));

        assertListEquals(expected, out);
    }

    @Test
    public void testKpiCounterGrouped() {
        List<Pair<WebLog, IntWritable>> out = null;
        driver.getConfiguration().setStrings("kpi.aggregation.group",
                "visitorId");
        driver.getConfiguration().setStrings("kpi.aggregation.type",
                WebLogType.WEB_LOG_COUNTER_GROUP.toString());
        Text input = new Text(
                "16737b1873ef03ad\thttp\thttp://tid.es/\ttid.es\t"
                        + "/\tnull\t09\t12\t2010\t00\t00\t-Microsoft-CryptoAPI/6.1\t"
                        + "-Microsoft-CryptoAPI/6.1\t-Microsoft-CryptoAPI/6.1\t-Microsoft-CryptoAPI/6.1\t"
                        + "GET\t304");

       try {
            out = this.driver.withInput(new LongWritable(0), input).run();

        } catch (IOException ioe) {
            fail();
        }
               
        WebLog key = WebLogFactory.getWebLog(null, null,
                WebLogType.WEB_LOG_COUNTER_GROUP);
        key.mainKey = "http\t-Microsoft-CryptoAPI/6.1\t09\t12\t2010";
        key.secondaryKey = "16737b1873ef03ad";

        List<Pair<WebLog, IntWritable>> expected = new ArrayList<Pair<WebLog, IntWritable>>();
        expected.add(new Pair<WebLog, IntWritable>(key, new IntWritable(1)));

        assertListEquals(expected, out);
    }
}
