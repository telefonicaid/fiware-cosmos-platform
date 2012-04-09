package es.tid.cosmos.profile.data;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserNavigation;

/**
 * Test case for UserNavigation
 *
 * @author sortega
 */
public class UserNavigationUtilTest {
    private UserNavigationUtil instance;

    @Before
    public void setUp() {
        this.instance = new UserNavigationUtil();
    }

    @Test
    public void testParse() {
        UserNavigation instance = UserNavigationUtil.parse("cfae4f24cb42c12d\t"
                + "http\t"
                + "http://xml.weather.com/mobile/android/factoids/delivery\t"
                + "weather.com\t/mobile/android/factoids/delivery/1130.xml\t"
                + "null\t30\t10\t2010\t0\t0\t-Java0\t-Java0\t-Java0\t-Java0\t"
                + "GET\t200");
        assertEquals("cfae4f24cb42c12d", instance.getUserId());
        assertEquals("http://xml.weather.com/mobile/android/factoids/delivery",
                     instance.getUrl());
    }
}
