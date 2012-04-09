package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.cosmos.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 * Test case for WebProfilingLogUtil
 *
 * @author dmicol
 */
public class WebProfilingLogUtilTest {
    @Test
    public void testCreate() {
        ProtobufWritable<WebProfilingLog> logWrapper = WebProfilingLogUtil.
                createAndWrap("cfae4f24cb42c12d", "http",
                              "http://xml.weather.com/mobile/android",
                              "weather.com", "/mobile/android/1130.xml", "",
                              "30/10/2010", "", "", "", "", "GET", "200", "");
        WebProfilingLog log = logWrapper.get();
        assertEquals("cfae4f24cb42c12d", log.getVisitorId());
        assertEquals("http://xml.weather.com/mobile/android", log.getFullUrl());
        assertEquals("GET", log.getMethod());
    }
}
