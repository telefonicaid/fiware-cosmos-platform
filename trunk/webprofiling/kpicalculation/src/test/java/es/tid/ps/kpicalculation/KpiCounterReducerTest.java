package es.tid.ps.kpicalculation;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.base.mapreduce.SingleKey;

/**
 * Test cases for the KpiCounterReducer class.
 */

public class KpiCounterReducerTest extends TestCase {
    private KpiCounterReducer reducer;
    private ReduceDriver<SingleKey, IntWritable, Text, IntWritable> driver;

    @Before
    public void setUp() {
        reducer = new KpiCounterReducer();
        driver = new ReduceDriver<SingleKey, IntWritable, Text, IntWritable>(
                reducer);
        driver.getConfiguration().setStrings("kpi.aggregation.fields",
                "protocol,urlDomain");
        driver.getConfiguration().setBoolean("kpi.aggregation.hashmap", true);
    }

    @Test
    public void testCounterReducer() throws Exception {
        SingleKey key = new SingleKey();
        key.setKey("http\ttid.es");

        List<IntWritable> values = new ArrayList<IntWritable>();
        values.add(new IntWritable(1));
        values.add(new IntWritable(3));
        values.add(new IntWritable(4));

        List<Pair<Text, IntWritable>> out = null;
        out = driver.withInputKey(key).withInputValues(values).run();

        List<Pair<Text, IntWritable>> expected = new ArrayList<Pair<Text, IntWritable>>();
        expected.add(new Pair<Text, IntWritable>(new Text("http\ttid.es"),
                new IntWritable(8)));

        assertListEquals(expected, out);
    }
}
