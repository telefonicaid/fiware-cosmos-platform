package es.tid.bdp.profile.categoryextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.data.UserNavigationUtil;
import es.tid.bdp.profile.data.WebProfilingLogUtil;
import es.tid.bdp.profile.generated.data.ProfileProtocol.UserNavigation;
import es.tid.bdp.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 * Test case for ProtobufCategoryExtractionMapper
 *
 * @author dmicol
 */
public class ProtobufCategoryExtractionMapperTest {
    private ProtobufCategoryExtractionMapper instance;
    private MapDriver<LongWritable, ProtobufWritable<WebProfilingLog>,
            BinaryKey, ProtobufWritable<UserNavigation>> driver;

    @Before
    public void setUp() {
        this.instance = new ProtobufCategoryExtractionMapper();
        this.driver = new MapDriver<LongWritable,
                ProtobufWritable<WebProfilingLog>, BinaryKey,
                ProtobufWritable<UserNavigation>>(this.instance);
    }

    @Test
    public void testMap() throws Exception {
        ProtobufWritable<WebProfilingLog> log =
                WebProfilingLogUtil.createAndWrap(
                        "cfae4f24cb42c12d", "",
                        "http://xml.weather.com/mobile/android", "", "", "",
                        "2010-10-30", "", "", "", "", "", "", "");
        BinaryKey key = new BinaryKey("cfae4f24cb42c12d", "2010-10-30");
        ProtobufWritable<UserNavigation> userNavigation =
                UserNavigationUtil.createAndWrap(
                        "cfae4f24cb42c12d",
                        "http://xml.weather.com/mobile/android",
                        "2010-10-30");
        this.driver
                .withInput(new LongWritable(0L), log)
                .withOutput(key, userNavigation)
                .runTest();
    }
}
