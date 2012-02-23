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

import es.tid.ps.base.mapreduce.BinaryKey;

/**
 * Test cases for the KpiCounterByReducer class
 */
public class KpiCounterByReducerTest extends TestCase {

    private KpiCounterByReducer reducer;
    private ReduceDriver<BinaryKey, IntWritable, Text, IntWritable> driver;

    @Before
    public void setUp() {
        reducer = new KpiCounterByReducer();
        driver = new ReduceDriver<BinaryKey, IntWritable, Text, IntWritable>(
                reducer);

        driver.getConfiguration().setStrings("kpi.aggregation.fields",
                "protocol,urlDomain");
        driver.getConfiguration().setStrings("kpi.aggregation.group",
                "visitorId");
    }

    @Test
    public void testCounterByReducer() throws Exception {
        BinaryKey key = new BinaryKey();
        key.setPrimaryKey("http\ttid.es");
        key.setSecondaryKey("16737b1873ef03ad");

        List<IntWritable> values = new ArrayList<IntWritable>();
        values.add(new IntWritable(1));
        values.add(new IntWritable(3));
        values.add(new IntWritable(4));

        List<Pair<Text, IntWritable>> out = null;
        out = driver.withInputKey(key).withInputValues(values).run();

        List<Pair<Text, IntWritable>> expected = new ArrayList<Pair<Text, IntWritable>>();
        expected.add(new Pair<Text, IntWritable>(new Text("http\ttid.es"),
                new IntWritable(1)));

        assertListEquals(expected, out);
    }
}
