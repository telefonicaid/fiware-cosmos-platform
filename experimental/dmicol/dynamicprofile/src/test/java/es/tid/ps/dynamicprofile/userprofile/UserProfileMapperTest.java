// <editor-fold defaultstate="collapsed" desc="Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.">
//
//   File        : UserProfileMapperTest.java
//
//   Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.
//
//   The copyright to the file(s) is property of Telefonica I+D.
//   The file(s) may be used and or copied only with the express written
//   consent of Telefonica I+D or in accordance with the terms and conditions
//   stipulated in the agreement/contract under which the files(s) have
//   been supplied.
//
// </editor-fold>
package es.tid.ps.dynamicprofile.userprofile;

import es.tid.ps.dynamicprofile.categoryextraction.CompositeKey;
import es.tid.ps.dynamicprofile.categoryextraction.CategoryInformation;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author sortega@tid.es
 */
public class UserProfileMapperTest {
    private MapDriver<CompositeKey, CategoryInformation, Text, CategoryCount> driver;

    @Before
    public void setUp() {
        driver = new MapDriver<CompositeKey, CategoryInformation, Text, CategoryCount>(
                new UserProfileMapper());
    }

    @Test
    public void mapTest() {
        String visitorId = "12345";
        String url = "http://tid.es";
        CompositeKey key = new CompositeKey(visitorId, url);
        CategoryInformation categories = new CategoryInformation(visitorId,
                url, 10, new String [] {"SERVICES", "NEWS"});
        driver.withInput(key, categories)
              .withOutput(new Text(visitorId), new CategoryCount("SERVICES", 10))
              .withOutput(new Text(visitorId), new CategoryCount("NEWS", 10))
              .runTest();
    }
}
