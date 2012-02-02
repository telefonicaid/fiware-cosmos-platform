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
package es.tid.ps.profile.userprofile;

import es.tid.ps.profile.categoryextraction.CategoryInformation;
import es.tid.ps.profile.categoryextraction.CompositeKey;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

/**
 * Test case for UserProfileMapper
 *
 * @author sortega@tid.es
 */
public class UserProfileMapperTest {
    private MapDriver<CompositeKey, CategoryInformation, 
            CompositeKey, CategoryCount> driver;

    @Before
    public void setUp() {
        driver = new MapDriver<CompositeKey, CategoryInformation, CompositeKey,
                CategoryCount>(new UserProfileMapper());
    }

    @Test
    public void mapTest() {
        String user = "12345";
        String url = "http://tid.es";
        String date = "2012-02-01";
        CompositeKey key = new CompositeKey(user, date);
        CategoryInformation categories = new CategoryInformation(user, url,
                date, 10, new String [] {"SERVICES", "NEWS"});
        driver.withInput(key, categories)
                .withOutput(new CompositeKey(user, date), 
                            new CategoryCount("SERVICES", 10))
                .withOutput(new CompositeKey(user, date), 
                            new CategoryCount("NEWS", 10))
                .runTest();
    }
}
