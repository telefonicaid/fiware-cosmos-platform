package es.tid.mapreduce.mobility2;

import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Calendar;
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

import es.tid.mapreduce.mobility2.data.Mobility2Counter;
import es.tid.mapreduce.mobility2.data.UserMobilityData;

@RunWith(BlockJUnit4ClassRunner.class)
public class Mobility2MapperTest extends TestCase {

    private Mapper<LongWritable, Text, Text, UserMobilityData> mapper;
    private MapDriver<LongWritable, Text, Text, UserMobilityData> driver;

    @Before
    public void setUp() {
        mapper = new Mobility2Mapper();
        driver = new MapDriver<LongWritable, Text, Text, UserMobilityData>(
                mapper);
        driver.getConfiguration().set("mapred.cache.files",
                "src/test/resources/cache_cells_data.txt");
    }

    @Test
    public void testBtsSource() {
        List<Pair<Text, UserMobilityData>> out = null;
        Calendar cal = Calendar.getInstance();
        cal.setTimeInMillis(1275382669000L);

        String line = "0442941212677|5520584478|33F430233D8A0F|0445537583140|2|01/06/2010|10:57:49|1|NOROAMI";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<Text, UserMobilityData>> expected = new ArrayList<Pair<Text, UserMobilityData>>();
        expected.add(new Pair<Text, UserMobilityData>(new Text("5520584478"),
                new UserMobilityData("13418", cal)));

        assertListEquals(expected, out);
    }
    
    @Test
    public void testBtsTarget() {
        List<Pair<Text, UserMobilityData>> out = null;
        Calendar cal = Calendar.getInstance();
        cal.setTimeInMillis(1275382669000L);

        String line = "33F430233D8A0F|5520584478|0442941212677|0445537583140|2|01/06/2010|10:57:49|1|NOROAMI";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<Text, UserMobilityData>> expected = new ArrayList<Pair<Text, UserMobilityData>>();
        expected.add(new Pair<Text, UserMobilityData>(new Text("5520584478"),
                new UserMobilityData("13418", cal)));

        assertListEquals(expected, out);
    }
    
    @Test
    public void testNoBts() {
        List<Pair<Text, UserMobilityData>> out = null;
        Calendar cal = Calendar.getInstance();
        cal.setTimeInMillis(1275382669000L);

        String line = "33F430233D8A0F|5520584478|33F430233D8A0F|0445537583140|2|01/06/2010|10:57:49|1|NOROAMI";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<Text, UserMobilityData>> expected = new ArrayList<Pair<Text, UserMobilityData>>();
        expected.add(new Pair<Text, UserMobilityData>(new Text("5520584478"),
                new UserMobilityData("", cal)));

        assertListEquals(expected, out);
    }
    
    @Test
    public void testParserError() {

        String line = "error line";

        try {
            driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        Counters expected = driver.getCounters();
        assertEquals(1L, expected.findCounter(Mobility2Counter.LINE_PARSER_CDRS_ERROR).getValue());
    }
    
    @Test
    public void testDateParserError() {
        String line = "33F430233D8A0F|5520584478|33F430233D8A0F|0445537583140|2|01/06/2010|10:MM:49|1|NOROAMI";

        try {
            driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        Counters expected = driver.getCounters();
        assertEquals(1L, expected.findCounter(Mobility2Counter.LINE_PARSER_CDRS_ERROR).getValue());
    }  
}