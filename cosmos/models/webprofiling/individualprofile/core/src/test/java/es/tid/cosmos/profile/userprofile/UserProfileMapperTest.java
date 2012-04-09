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
package es.tid.cosmos.profile.userprofile;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import static es.tid.cosmos.profile.data.CategoryCountUtil.createAndWrap;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryInformation;

/**
 * Test case for UserProfileMapper
 *
 * @author sortega@tid.es
 */
public class UserProfileMapperTest {
    private UserProfileMapper instance;
    private MapDriver<BinaryKey, ProtobufWritable<CategoryInformation>,
        BinaryKey, ProtobufWritable<CategoryCount>> driver;

    @Before
    public void setUp() {
        this.instance = new UserProfileMapper();
        this.driver = new MapDriver<BinaryKey, ProtobufWritable<CategoryInformation>,
                BinaryKey, ProtobufWritable<CategoryCount>>(instance);
    }

    @Test
    public void mapTest() throws Exception {
        String user = "12345";
        String url = "http://tid.es";
        String date = "2012-02-01";
        BinaryKey key = new BinaryKey(user, date);
        CategoryInformation categories = CategoryInformation
                .newBuilder()
                .setUserId(user)
                .setUrl(url)
                .setDate(date)
                .setCount(10L)
                .addAllCategories(asList("SERVICES", "NEWS"))
                .build();
        ProtobufWritable<CategoryInformation> wrapper = new ProtobufWritable();
        wrapper.setConverter(CategoryInformation.class);
        wrapper.set(categories);

        this.driver
                .withInput(key, wrapper)
                .withOutput(key, createAndWrap("SERVICES", 10L))
                .withOutput(key, createAndWrap("NEWS", 10L))
                .runTest();
    }
}
