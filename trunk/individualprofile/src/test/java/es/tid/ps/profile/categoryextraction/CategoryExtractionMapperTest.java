package es.tid.ps.profile.categoryextraction;

import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.data.ProfileProtocol.UserNavigation;

/**
 * Test case for CategoryExtractionMapper
 *
 * @author sortega
 */
public class CategoryExtractionMapperTest {
    private CategoryExtractionMapper instance;
    private MapDriver<LongWritable, Text, BinaryKey,
            ProtobufWritable<UserNavigation>> driver;

    @Before
    public void setUp() throws Exception {
        instance = new CategoryExtractionMapper();
        driver = new MapDriver<LongWritable, Text, BinaryKey,
                ProtobufWritable<UserNavigation>> (instance);
    }

    @Test
    public void mapLogLine() throws Exception {
        String input = "cfae4f24cb42c12d\thttp\t"
                + "http://xml.weather.com/mobile/android/factoids/delivery\t"
                + "weather.com\t/mobile/android/factoids/delivery/1130.xml\t"
                + "null\t30\t10\t2010\t0\t0\t-Java0\t-Java0\t-Java0\t-Java0\t"
                + "GET\t200";
        List<Pair<BinaryKey, ProtobufWritable<UserNavigation>>> output =
                driver.withInput(new LongWritable(0), new Text(input)).run();

        assertEquals("Only one pair", 1, output.size());
    }
}
