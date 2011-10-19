package es.tid.ps.mapreduce.sna;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

/**
 * Test cases for the Social Graph reducer.
 * 
 * @author rgc@tid.es
 * 
 */
public class SocialGraphReducerTest extends TestCase {

    private Reducer<Text, Text, Text, ArrayListWritable> reducer;
    private ReduceDriver<Text, Text, Text, ArrayListWritable> driver;

    @Before
    public void setUp() {
        reducer = new SocialGraphReducer();
        driver = new ReduceDriver<Text, Text, Text, ArrayListWritable>(reducer);
    }

    @Test
    public void testOneOffset() {
        List<Pair<Text, ArrayListWritable>> out = null;
        Text key = new Text("5");
        List<Text> values = new ArrayList<Text>();

        values.add(new Text("5"));
        values.add(new Text("7"));
        values.add(new Text("6"));
        values.add(new Text("8"));

        try {
            out = driver.withInputKey(key).withInputValues(values).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, ArrayListWritable>> expected = new ArrayList<Pair<Text, ArrayListWritable>>();

        String[] vv = { new String("5"), new String("7"), new String("6"),
                new String("8") };

        expected.add(new Pair<Text, ArrayListWritable>(new Text("5"),
                new ArrayListWritable(vv)));

        //TODO Iḿ not sure that this test response ok
        assertListEquals(expected, out);
    }
}