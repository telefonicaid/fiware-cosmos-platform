package es.tid.ps.kpicalculation;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.NoSuchElementException;

import junit.framework.TestCase;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counters;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.kpicalculation.data.WebLog;
import es.tid.ps.kpicalculation.data.WebLogFactory;
import es.tid.ps.kpicalculation.data.WebLogType;


/**
 * Test cases for the MobilityReducer.
 */

public class KpiCounterByReducerTest extends  TestCase{

    private KpiCounterByReducer reducer;
    private ReduceDriver<WebLog, IntWritable, Text,IntWritable> driver;

    @Before
    public void setUp() {
        reducer = new KpiCounterByReducer();
        driver = new ReduceDriver<WebLog, IntWritable, Text,IntWritable>(
                reducer);
        
        driver.getConfiguration().setStrings("kpi.aggregation.fields","protocol,urlDomain");
        driver.getConfiguration().setStrings("kpi.aggregation.group", "visitorId");
        
        
    }

    @Test
    public void testOneWorkHour() {
        List<Pair<Text,IntWritable>> out = null;
        String actualUser = "33F43075348D57";

       
        try {
            List<IntWritable> values = new ArrayList<IntWritable>();
            Collection<String> col = new ArrayList();
            col.add("protocol");
            WebLog p = WebLogFactory.getWebLog(col, "visitorId",
                    WebLogType.WEB_LOG_COUNTER_GROUP);
            p.mainKey = "http\ttid.es";
            p.secondaryKey = "16737b1873ef03ad";
            values.add(new IntWritable(1));
            values.add(new IntWritable(3));
            values.add(new IntWritable(4));
            
                        out = driver.withInputKey(p)
                    .withInputValues(values).run();
                        
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, IntWritable>> expected = new ArrayList<Pair<Text, IntWritable>>();
        expected.add(new Pair<Text, IntWritable>(new Text("http\ttid.es"), new IntWritable(1)));
        
        assertListEquals(expected, out);
    }

    
}
