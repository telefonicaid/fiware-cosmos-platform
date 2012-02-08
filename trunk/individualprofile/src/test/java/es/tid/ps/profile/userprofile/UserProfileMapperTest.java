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

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.categoryextraction.CategoryInformation;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

/**
 * Test case for UserProfileMapper
 *
 * @author sortega@tid.es
 */
public class UserProfileMapperTest {
    private MapDriver<BinaryKey, CategoryInformation, BinaryKey, CategoryCount>
            driver;

    @Before
    public void setUp() {
        driver = new MapDriver<BinaryKey, CategoryInformation, BinaryKey,
                CategoryCount>(new UserProfileMapper());
    }

    @Test
    public void mapTest() {
        String user = "12345";
        String url = "http://tid.es";
        String date = "2012-02-01";
        BinaryKey key = new BinaryKey(user, date);
        CategoryInformation categories = new CategoryInformation(user, url,
                date, 10, new String [] {"SERVICES", "NEWS"});
        driver.withInput(key, categories)
                .withOutput(new BinaryKey(user, date), 
                            new CategoryCount("SERVICES", 10))
                .withOutput(new BinaryKey(user, date), 
                            new CategoryCount("NEWS", 10))
                .runTest();
    }
}
