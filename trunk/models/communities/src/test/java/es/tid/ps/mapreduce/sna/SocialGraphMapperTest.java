package es.tid.ps.mapreduce.sna;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

/**
 * Test cases for the Social Graph mapper.
 * 
 * @author rgc@tid.es
 * 
 */
public class SocialGraphMapperTest extends TestCase {

    private Mapper<LongWritable, Text, Text, Text> mapper;
    private MapDriver<LongWritable, Text, Text, Text> driver;

    @Before
    public void setUp() {
        mapper = new SocialGraphMapper();
        driver = new MapDriver<LongWritable, Text, Text, Text>(mapper);
    }

    @Test
    public void testMultiWords() {
        List<Pair<Text, Text>> out = null;

        try {
            out = driver.withInput(new LongWritable(1), new Text("a|1|2")).run();

        } catch (IOException ioe) {
            fail();
        }

        List<Pair<Text, Text>> expected = new ArrayList<Pair<Text, Text>>();
        expected.add(new Pair<Text, Text>(new Text("1"), new Text("2")));
        expected.add(new Pair<Text, Text>(new Text("2"), new Text("1")));

        assertListEquals(expected, out);
    }
}
