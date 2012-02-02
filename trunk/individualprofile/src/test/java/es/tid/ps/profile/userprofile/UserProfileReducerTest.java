// <editor-fold defaultstate="collapsed" desc="Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.">
//
//   File        : UserProfileReducerTest.java
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

import es.tid.ps.profile.categoryextraction.CompositeKey;
import static java.util.Arrays.asList;
import java.util.List;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 * Test case for UserProfileReducer
 *
 * @author sortega@tid.es
 */
public class UserProfileReducerTest {

    private ReduceDriver<CompositeKey, CategoryCount, Text, UserProfile> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<CompositeKey, CategoryCount, 
                Text, UserProfile>(new UserProfileReducer());
    }

    @Test
    public void testReduce() throws Exception {
        String visitorId = "12345";

        List<Pair<Text, UserProfile>> results = driver.withInput(
                new CompositeKey(visitorId, "2012-02-01"),
                asList(new CategoryCount("SPORT", 10),
                       new CategoryCount("NEWS", 10),
                       new CategoryCount("SPORT", 2))).run();

        assertEquals("One result", 1, results.size());
        Pair<Text, UserProfile> result = results.get(0);
        assertEquals(visitorId, result.getFirst().toString());
        UserProfile profile = result.getSecond();
        assertEquals(12, profile.getCount(new CategoryCount("SPORT", 0)));
        assertEquals(10, profile.getCount(new CategoryCount("NEWS", 0)));
    }
}