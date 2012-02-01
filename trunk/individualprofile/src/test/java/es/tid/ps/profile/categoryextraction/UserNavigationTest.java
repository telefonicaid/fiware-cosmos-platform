package es.tid.ps.profile.categoryextraction;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 * Test case for UserNavigation
 * 
 * @author sortega
 */
public class UserNavigationTest {
    
    @Test
    public void testParse() {
        UserNavigation instance = new UserNavigation();
        instance.parse("cfae4f24cb42c12d\thttp\t"
                + "http://xml.weather.com/mobile/android/factoids/delivery\t"
                + "weather.com\t/mobile/android/factoids/delivery/1130.xml\t"
                + "null\t30\t10\t2010\t0\t0\t-Java0\t-Java0\t-Java0\t-Java0\t"
                + "GET\t200");
        assertEquals("cfae4f24cb42c12d", instance.getVisitorId());
        assertEquals("http://xml.weather.com/mobile/android/factoids/delivery",
                     instance.getFullUrl());
    }
}
