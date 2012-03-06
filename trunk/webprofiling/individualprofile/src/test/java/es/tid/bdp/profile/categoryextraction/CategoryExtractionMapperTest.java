package es.tid.bdp.profile.categoryextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.data.ProfileProtocol.UserNavigation;
import es.tid.bdp.profile.data.UserNavigationUtil;

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
        driver.withInput(new LongWritable(0), new Text(input))
                .withOutput(new BinaryKey("cfae4f24cb42c12d", "2010-10-30"),
                UserNavigationUtil.createAndWrap("cfae4f24cb42c12d",
                "http://xml.weather.com/mobile/android/factoids/delivery",
                "2010-10-30"))
                .runTest();
    }
}
