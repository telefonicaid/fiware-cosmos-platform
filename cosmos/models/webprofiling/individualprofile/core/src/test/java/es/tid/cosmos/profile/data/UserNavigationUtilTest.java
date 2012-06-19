package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserNavigation;

/**
 * Test case for UserNavigation
 *
 * @author sortega
 */
public class UserNavigationUtilTest {
    @Test
    public void shouldBeUtilityClass() {
        assertUtilityClass(UserNavigationUtil.class);
    }

    @Test
    public void testCreate() {
        final String userId = "cfae4f24cb42c12d";
        final String url = "http://www.mutxamel.org/parks";
        final String date = "30/10/2010";
        ProtobufWritable<UserNavigation> navWrapper =
                UserNavigationUtil.createAndWrap(userId, url, date);
        UserNavigation nav = navWrapper.get();
        assertEquals(userId, nav.getUserId());
        assertEquals(url, nav.getUrl());
        assertEquals(date, nav.getDate());
    }

    @Test
    public void testParse() {
        UserNavigation nav = UserNavigationUtil.parse("cfae4f24cb42c12d\t"
                + "http\t"
                + "http://xml.weather.com/mobile/android/factoids/delivery\t"
                + "weather.com\t/mobile/android/factoids/delivery/1130.xml\t"
                + "null\t30\t10\t2010\t0\t0\t-Java0\t-Java0\t-Java0\t-Java0\t"
                + "GET\t200");
        assertEquals("cfae4f24cb42c12d", nav.getUserId());
        assertEquals("http://xml.weather.com/mobile/android/factoids/delivery",
                     nav.getUrl());
    }
}
