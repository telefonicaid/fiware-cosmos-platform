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
package es.tid.cosmos.profile.userprofile;

import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.profile.data.CategoryCountUtil;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserProfile;

/**
 * Test case for UserProfileReducer
 *
 * @author sortega@tid.es
 */
public class UserProfileReducerTest {
    private ReduceDriver<BinaryKey, ProtobufWritable<CategoryCount>,
            Text, ProtobufWritable<UserProfile>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<BinaryKey, ProtobufWritable<CategoryCount>,
            Text, ProtobufWritable<UserProfile>>(new UserProfileReducer());
    }

    @Test
    public void testReduce() throws Exception {
        String visitorId = "12345";

        List<Pair<Text, ProtobufWritable<UserProfile>>> results = driver.withInput(
                new BinaryKey(visitorId, "2012-02-01"),
                asList(CategoryCountUtil.createAndWrap("SPORT", 10),
                       CategoryCountUtil.createAndWrap("NEWS", 10),
                       CategoryCountUtil.createAndWrap("SPORT", 2))).run();

        assertEquals("One result", 1, results.size());
        Pair<Text, ProtobufWritable<UserProfile>> result = results.get(0);
        assertEquals(visitorId, result.getFirst().toString());

        final ProtobufWritable<UserProfile> wrappedProfile = result.getSecond();
        wrappedProfile.setConverter(UserProfile.class);
        UserProfile profile = wrappedProfile.get();

        final CategoryCount firstCount = profile.getCounts(0);
        assertEquals("SPORT", firstCount.getName());
        assertEquals(12, firstCount.getCount());

        final CategoryCount secondCount = profile.getCounts(1);
        assertEquals("NEWS", secondCount.getName());
        assertEquals(10, secondCount.getCount());
    }
}
