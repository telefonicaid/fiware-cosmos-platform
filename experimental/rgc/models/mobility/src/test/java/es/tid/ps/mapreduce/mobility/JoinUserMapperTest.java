package es.tid.ps.mapreduce.mobility;

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
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;
import es.tid.ps.mapreduce.mobility.data.FileType;

/**
 * Test case for JoinUserMapper
 * 
 * @author rgc
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class JoinUserMapperTest extends TestCase {

    private Mapper<LongWritable, Text, CompositeKey, Text> mapper;
    private MapDriver<LongWritable, Text, CompositeKey, Text> driver;

    @Before
    public void setUp() {
        mapper = new JoinUserMapper();
        driver = new MapDriver<LongWritable, Text, CompositeKey, Text>(mapper);
    }

    @Test
    public void testEmpty() {
        List<Pair<CompositeKey, Text>> out = null;

        try {
            out = driver.withInput(new LongWritable(0), new Text("")).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<CompositeKey, Text>> expected = new ArrayList<Pair<CompositeKey, Text>>();

        assertListEquals(expected, out);
    }

    @Test
    public void testOneCdr() {
        List<Pair<CompositeKey, Text>> out = null;
        String line = "1000025948|P|28/06/2010||||||15%||05349|||VENDEDOR T.P.|-4|-4";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<CompositeKey, Text>> expected = new ArrayList<Pair<CompositeKey, Text>>();
        expected.add(new Pair<CompositeKey, Text>(new CompositeKey(
                "1000025948", FileType.FILE_USERS.getValue()), new Text("")));

        assertListEquals(expected, out);
    }
}