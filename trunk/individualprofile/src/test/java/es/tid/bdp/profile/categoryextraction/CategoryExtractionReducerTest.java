// <editor-fold defaultstate="collapsed" desc="Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.">
//
//   File        : CategoryExtractionReducerTest.java
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
package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.Calendar;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer.Context;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.data.ProfileProtocol.CategoryInformation;
import es.tid.bdp.profile.data.ProfileProtocol.UserNavigation;
import es.tid.bdp.profile.data.UserNavigationUtil;
import es.tid.bdp.profile.dictionary.Categorization;
import es.tid.bdp.profile.dictionary.CategorizationResult;

/**
 * Test case for CategoryExtractionReducer
 *
 * @author sortega@tid.es
 */
public class CategoryExtractionReducerTest {
    private CategoryExtractionReducer instance;
    private ReduceDriver<BinaryKey, ProtobufWritable<UserNavigation>, BinaryKey,
            ProtobufWritable<CategoryInformation>> driver;

    @Before
    public void setUp() {
        this.instance = new CategoryExtractionReducer() {
            @Override
            protected void setupDictionary(Context context) throws IOException {
                // Avoid loading the real dictionary
            }

            @Override
            protected Categorization categorize(String url) {
                Categorization categorization = new Categorization();
                if (url.equals("http://www.marca.es/basket")) {
                    categorization.setResult(CategorizationResult.KNOWN_URL);
                    categorization.setCategories(
                            new String[]{"SPORTS", "NEWS"});
                } else if (url.equals("http://www.mutxamel.org")) {
                    categorization.setResult(CategorizationResult.UNKNOWN_URL);
                } else if (url.equals("http://www.realmadrid.com")) {
                    categorization.setResult(
                            CategorizationResult.IRRELEVANT_URL);
                } else if (url.isEmpty()) {
                    categorization.setResult(
                            CategorizationResult.GENERIC_FAILURE);
                } else {
                    assert false;
                }
                return categorization;
            }
        };
        this.driver = new ReduceDriver<BinaryKey, ProtobufWritable<UserNavigation>,
                BinaryKey, ProtobufWritable<CategoryInformation>>(this.instance);
    }

    @Test
    public void testKnownUrl() throws Exception {
        String visitorId = "CA003B";
        String fullUrl = "http://www.marca.es/basket";
        Calendar date = Calendar.getInstance();
        BinaryKey key = new BinaryKey(visitorId, date.toString());
        UserNavigation nav =
                UserNavigationUtil.create(visitorId, fullUrl, date.toString());
        ProtobufWritable<UserNavigation> wrapper =
                ProtobufWritable.newInstance(UserNavigation.class);
        wrapper.setConverter(UserNavigation.class);
        wrapper.set(nav);

        List<Pair<BinaryKey, ProtobufWritable<CategoryInformation>>> results =
                driver.withInput(key, asList(wrapper, wrapper)).run();

        assertEquals(1, results.size());
        final Pair<BinaryKey, ProtobufWritable<CategoryInformation>> result =
                results.get(0);
        assertEquals(key, result.getFirst());

        CategoryInformation expectedCategoryInformation = CategoryInformation.
                newBuilder().setUserId(visitorId).setUrl(fullUrl).setDate(date.
                toString()).setCount(2L).addAllCategories(asList("SPORTS",
                                                                 "NEWS")).build();

        ProtobufWritable<CategoryInformation> wrappedCategoryInfo =
                result.getSecond();
        wrappedCategoryInfo.setConverter(CategoryInformation.class);
        assertEquals(expectedCategoryInformation, wrappedCategoryInfo.get());
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.KNOWN_VISITS).getValue());
    }

    @Test
    public void testUnknownUrl() throws Exception {
        String visitorId = "CA003C";
        String fullUrl = "http://www.mutxamel.org";
        String date = "02/01/2012";
        BinaryKey key = new BinaryKey(visitorId, date);
        UserNavigation nav =
                UserNavigationUtil.create(visitorId, fullUrl, date.toString());
        ProtobufWritable<UserNavigation> wrapper =
                ProtobufWritable.newInstance(UserNavigation.class);
        wrapper.set(nav);

        driver.withInput(key, asList(wrapper, wrapper))
                .runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.UNKNOWN_VISITS).getValue());
    }

    @Test
    public void testIrrelevantUrl() throws Exception {
        String visitorId = "CA003D";
        String fullUrl = "http://www.realmadrid.com";
        Calendar date = Calendar.getInstance();
        BinaryKey key = new BinaryKey(visitorId, date.toString());
        UserNavigation nav =
                UserNavigationUtil.create(visitorId, fullUrl, date.toString());
        ProtobufWritable<UserNavigation> wrapper =
                ProtobufWritable.newInstance(UserNavigation.class);
        wrapper.set(nav);

        driver.withInput(key, asList(wrapper, wrapper))
                .runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.IRRELEVANT_VISITS).getValue());
    }

    @Test
    public void testGenericFailure() throws Exception {
        String visitorId = "CA003E";
        String fullUrl = "";
        Calendar date = Calendar.getInstance();
        BinaryKey key = new BinaryKey(visitorId, date.toString());
        UserNavigation nav =
                UserNavigationUtil.create(visitorId, fullUrl, date.toString());
        ProtobufWritable<UserNavigation> wrapper =
                ProtobufWritable.newInstance(UserNavigation.class);
        wrapper.set(nav);

        driver.withInput(key, asList(wrapper, wrapper))
                .runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.UNPROCESSED_VISITS).getValue());
    }
}
