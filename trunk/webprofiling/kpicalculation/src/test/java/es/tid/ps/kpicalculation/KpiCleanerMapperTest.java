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
 */
public class KpiCleanerMapperTest extends TestCase {

    private Mapper<LongWritable, Text, LongWritable, Text> mapper;
    private MapDriver<LongWritable, Text, LongWritable, Text> driver;

    @Before
    protected void setUp() {
        mapper = new KpiCleanerMapper();
        driver = new MapDriver<LongWritable, Text, LongWritable, Text>(mapper);
        driver.getConfiguration().addResource("kpi-filtering.xml");
    }

    @Test
    public void testAllowedExtension() {
        List<Pair<LongWritable, Text>> out = null;
        String input = "16737b1873ef03ad	http://www.tid.es/index.html	1Dec2010000001	304	application/pkix-crl	-Microsoft-CryptoAPI/6.1	GET";
        String output = "16737b1873ef03ad	http	http://tid.es/	tid.es	/	null	1Dec2010000001	1Dec2010000001	-Microsoft-CryptoAPI/6.1	-Microsoft-CryptoAPI/6.1	-Microsoft-CryptoAPI/6.1	-Microsoft-CryptoAPI/6.1	GET	304";

        try {
            out = driver.withInput(new LongWritable(0), new Text(input)).run();
        } catch (IOException ioe) {
            fail();
        }

        List<Pair<LongWritable, Text>> expected = new ArrayList<Pair<LongWritable, Text>>();
        expected.add(new Pair<LongWritable, Text>(new LongWritable(0),
                new Text(output)));

        assertListEquals(expected, out);
    }

    @Test
    public void testForbiddenExtension() {
        List<Pair<LongWritable, Text>> out = null;
        String line = "16737b1873ef03ad	http://www.tid.co.uk/foto.jpg	1Dec2010000001	304	application/pkix-crl	-Microsoft-CryptoAPI/6.1	GET";
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
        String line = "16737b1873ef03ad	http://sexsearch.com/asfad	1Dec2010000001	304	application/pkix-crl	-Microsoft-CryptoAPI/6.1	GET";

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
        String line = "16737b1873ef03ad	http://pornhub.com/t1/video	1Dec2010000001	304	application/pkix-crl	-Microsoft-CryptoAPI/6.1	GET";

        try {
            out = driver.withInput(new LongWritable(0), new Text(line)).run();
        } catch (IOException ioe) {
            fail();
        }
        List<Pair<LongWritable, Text>> expected = new ArrayList<Pair<LongWritable, Text>>();

        assertListEquals(expected, out);
    }
}
