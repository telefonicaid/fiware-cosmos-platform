package es.tid.ps.mapreduce.mobility;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counters;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;
import es.tid.ps.mapreduce.mobility.data.FileType;
import es.tid.ps.mapreduce.mobility.data.MobilityCounter;

/**
 * Test case for joinCdrsMapper
 * 
 * @author rgc
 * 
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class JoinCdrsMapperTest extends TestCase {

    private Mapper<LongWritable, Text, CompositeKey, Text> mapper;
    private MapDriver<LongWritable, Text, CompositeKey, Text> driver;

    @Before
    public void setUp() {
        mapper = new JoinCdrsMapper();
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
    public void testOneUser() {
        List<Pair<CompositeKey, Text>> out = null;
        String line = "33F430233D8A0F|5520584478|33F430233D8A0F|0445537583140|2|01/06/2010|10:57:49|1|NOROAMI";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();            
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<CompositeKey, Text>> expected = new ArrayList<Pair<CompositeKey, Text>>();
        expected.add(new Pair<CompositeKey, Text>(new CompositeKey(
                "5520584478", FileType.FILE_CDRS.getValue()), new Text(line)));

        assertListEquals(expected, out);
    }
    
    @Test
    public void testParseLineError() {
        String line = "33F430233D8A0F";

        try {
            driver.withInput(new LongWritable(0), new Text(line)).run();  
         
        } catch (IOException ioe) {
            fail();
        }
        Counters expected = driver.getCounters();
        assertEquals(1L, expected.findCounter(MobilityCounter.LINE_PARSER_CDRS_ERROR).getValue());
    }
}
