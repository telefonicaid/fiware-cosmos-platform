package es.tid.cosmos.kpicalculation;

import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 * Test cases for the KpiCounterByReducer class
 */
public class KpiCounterByReducerTest {

    private KpiCounterByReducer reducer;
    private ReduceDriver<CompositeKey, IntWritable, Text, IntWritable> driver;

    @Before
    public void setUp() {
        this.reducer = new KpiCounterByReducer();
        this.driver = new ReduceDriver<CompositeKey, IntWritable, Text,
                                       IntWritable>(this.reducer);

        this.driver.getConfiguration().setStrings("kpi.aggregation.fields",
                                                  "protocol,urlDomain");
        this.driver.getConfiguration().setStrings("kpi.aggregation.group",
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

        List<Pair<Text, IntWritable>> out =
                this.driver.withInputKey(key).withInputValues(values).run();

        List<Pair<Text, IntWritable>> expected =
                new ArrayList<Pair<Text, IntWritable>>();
        expected.add(new Pair<Text, IntWritable>(new Text("http\ttid.es"),
                new IntWritable(1)));

        assertListEquals(expected, out);
    }
}
