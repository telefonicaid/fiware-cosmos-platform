package es.tid.ps.kpicalculation;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;

import org.junit.Before;
import org.junit.Test;

import junit.framework.TestCase;

/**
 * Test case for KpiCleanerMapper
 * 
 * @author javierb
 * 
 */
public class KpiCleanerMapperTest extends TestCase {

    private Mapper<LongWritable, Text, LongWritable, Text> mapper;
    private MapDriver<LongWritable, Text, LongWritable, Text> driver;

    @Before
    protected void setUp() {
        mapper = new KpiCleanerMapper();
        driver = new MapDriver<LongWritable, Text, LongWritable, Text>(mapper);
    }

   
    @Test
    public void testAllowedExtension() {
        List<Pair<LongWritable, Text>> out = null;
        String line = "http://www.tid.es";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<LongWritable, Text>> expected = new ArrayList<Pair<LongWritable, Text>>();
        expected.add(new Pair<LongWritable, Text>(new LongWritable(0), new Text(line)));

        assertListEquals(expected, out);
    }

    @Test
    public void testForbiddenExtension() {
        List<Pair<LongWritable, Text>> out = null;
        String line = "http://www.tid.es/foto.jpg";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<LongWritable, Text>> expected = new ArrayList<Pair<LongWritable, Text>>();
       

        assertListEquals(expected, out);
    }
    
    @Test
    public void testThirdPartyDomain() {
        List<Pair<LongWritable, Text>> out = null;
        String line = "http://sexsearch.com/asfad";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<LongWritable, Text>> expected = new ArrayList<Pair<LongWritable, Text>>();
       

        assertListEquals(expected, out);
    }
    
    @Test
    public void testPersonalInfoDomain() {
        List<Pair<LongWritable, Text>> out = null;
        String line = "http://pornhub.com/asfad";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<LongWritable, Text>> expected = new ArrayList<Pair<LongWritable, Text>>();
       

        assertListEquals(expected, out);
    }

}
