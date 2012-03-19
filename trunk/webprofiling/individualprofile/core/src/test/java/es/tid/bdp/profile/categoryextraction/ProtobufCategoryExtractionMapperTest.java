package es.tid.bdp.profile.categoryextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.base.mapreduce.TernaryKey;
import es.tid.bdp.profile.data.WebProfilingLogUtil;
import es.tid.bdp.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 * Test case for ProtobufCategoryExtractionMapper
 *
 * @author dmicol
 */
public class ProtobufCategoryExtractionMapperTest {
    private ProtobufCategoryExtractionMapper instance;
    private MapDriver<LongWritable, ProtobufWritable<WebProfilingLog>,
            TernaryKey, LongWritable> driver;

    @Before
    public void setUp() {
        this.instance = new ProtobufCategoryExtractionMapper();
        this.driver = new MapDriver<LongWritable, ProtobufWritable<
                WebProfilingLog>, TernaryKey, LongWritable> (this.instance);
    }

    @Test
    public void testMap() throws Exception {
        final String expectedUrl = "http://xml.weather.com/mobile/android/";
        final String expectedUserId = "cfae4f24cb42c12d";
        final String expectedDate = "2010-10-30";
        ProtobufWritable<WebProfilingLog> log = WebProfilingLogUtil
                .createAndWrap(expectedUserId, "", expectedUrl, "", "", "",
                               expectedDate, "", "", "", "", "", "", "");
        this.driver.withInput(new LongWritable(0L), log)
                .withOutput(new TernaryKey(expectedUserId, expectedDate,
                                           expectedUrl),
                            new LongWritable(1L))
                .runTest();
    }
}
