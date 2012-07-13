package es.tid.cosmos.profile.categoryextraction;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.TernaryKey;

/**
 * Test case for CategoryExtractionMapper
 *
 * @author sortega
 */
public class TextCategoryExtractionMapperTest {
    private TextCategoryExtractionMapper instance;
    private MapDriver<LongWritable, Text, TernaryKey, LongWritable> driver;

    @Before
    public void setUp() throws Exception {
        instance = new TextCategoryExtractionMapper();
        driver = new MapDriver<LongWritable, Text, TernaryKey, LongWritable>
                (instance);
    }

    @Test
    public void mapLogLine() throws Exception {
        String input = "cfae4f24cb42c12d\thttp\t"
                + "http://xml.weather.com/mobile/android/factoids/delivery\t"
                + "weather.com\t/mobile/android/factoids/delivery/1130.xml\t"
                + "null\t30\t10\t2010\t0\t0\t-Java0\t-Java0\t-Java0\t-Java0\t"
                + "GET\t200";
        final String expectedUrl = "http://xml.weather.com/mobile/android/factoids/delivery";
        final String expectedUserId = "cfae4f24cb42c12d";
        final String expectedDate = "2010-10-30";
        driver.withInput(new LongWritable(0), new Text(input))
                .withOutput(new TernaryKey(expectedUserId, expectedDate,
                                         expectedUrl),
                            new LongWritable(1L))
                .runTest();
    }
}
