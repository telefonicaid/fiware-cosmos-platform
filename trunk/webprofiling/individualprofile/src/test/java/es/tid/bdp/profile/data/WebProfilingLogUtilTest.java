package es.tid.bdp.profile.data;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 * Test case for WebProfilingLogUtil
 *
 * @author dmicol
 */
public class WebProfilingLogUtilTest {
    private WebProfilingLogUtil instance;

    @Before
    public void setUp() {
        this.instance = new WebProfilingLogUtil();
    }

    @Test
    public void testCreate() {
        WebProfilingLog log = WebProfilingLogUtil.create(
                "cfae4f24cb42c12d", "http",
                "http://xml.weather.com/mobile/android/factoids/delivery",
                "weather.com", "/mobile/android/factoids/delivery/1130.xml",
                "", "30/10/2010", "", "", "", "", "GET", "200", "");
        assertEquals("cfae4f24cb42c12d", log.getVisitorId());
        assertEquals("http://xml.weather.com/mobile/android/factoids/delivery",
                     log.getFullUrl());
        assertEquals("GET", log.getMethod());
    }
}
