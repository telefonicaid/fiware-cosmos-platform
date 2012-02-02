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
package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import org.apache.hadoop.mapreduce.Reducer.Context;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.profile.dictionary.Categorization;
import es.tid.ps.profile.dictionary.CategorizationResult;
import java.util.Calendar;

/**
 * Test case for CategoryExtractionReducer
 *
 * @author sortega@tid.es
 */
public class CategoryExtractionReducerTest {
    private CategoryExtractionReducer instance;
    private ReduceDriver<CompositeKey, UserNavigation, CompositeKey,
            CategoryInformation> driver;

    @Before
    public void setUp() {
        instance = new CategoryExtractionReducer() {
            @Override
            protected void setupDictionary(Context context) throws IOException {
                // Avoid loading the real dictionary
            }

            @Override
            protected Categorization categorize(String url) {
                Categorization categorization = new Categorization();
                if (url.equals("http://www.marca.es/basket")) {
                    categorization.result = CategorizationResult.KNOWN_URL;
                    categorization.categories =
                            new String[] { "SPORTS", "NEWS" };
                } else if (url.equals("http://www.mutxamel.org")) {
                    categorization.result = CategorizationResult.UNKNOWN_URL;
                } else if (url.equals("http://www.realmadrid.com")) {
                    categorization.result = CategorizationResult.IRRELEVANT_URL;
                } else if (url.isEmpty()) {
                    categorization.result =
                            CategorizationResult.GENERIC_FAILURE;
                } else {
                    assert false;
                }
                return categorization;
            }
        };
        driver = new ReduceDriver<CompositeKey, UserNavigation, CompositeKey,
                CategoryInformation>(instance);
    }

    @Test
    public void testKnownUrl() throws Exception {
        String visitorId = "CA003B";
        String fullUrl = "http://www.marca.es/basket";
        Calendar date = Calendar.getInstance();
        CompositeKey key = new CompositeKey(visitorId, date.toString());
        UserNavigation nav = new UserNavigation(visitorId, fullUrl, date);

        CategoryInformation expectedCategoryInformation =
                new CategoryInformation(visitorId, fullUrl, date.toString(),
                        2, new String[] {"SPORTS", "NEWS"});

        driver.withInput(key, asList(nav, nav))
              .withOutput(key, expectedCategoryInformation)
              .runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.KNOWN_VISITS).getValue());
    }
    
    @Test
    public void testUnknownUrl() throws Exception {
        String visitorId = "CA003C";
        String fullURL = "http://www.mutxamel.org";
        CompositeKey key = new CompositeKey(visitorId, "02/01/2012");
        UserNavigation nav = new UserNavigation();
        nav.setFullUrl(fullURL);

        driver.withInput(key, asList(nav, nav)).runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.UNKNOWN_VISITS).getValue());
    }
        
    @Test
    public void testIrrelevantUrl() throws Exception {
        String visitorId = "CA003D";
        String fullUrl = "http://www.realmadrid.com";
        Calendar date = Calendar.getInstance();
        CompositeKey key = new CompositeKey(visitorId, date.toString());
        UserNavigation nav = new UserNavigation(visitorId, fullUrl, date);

        driver.withInput(key, asList(nav, nav)).runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.IRRELEVANT_VISITS).getValue());
    }
            
    @Test
    public void testGenericFailure() throws Exception {
        String visitorId = "CA003E";
        String fullUrl = "";
        Calendar date = Calendar.getInstance();
        CompositeKey key = new CompositeKey(visitorId, date.toString());
        UserNavigation nav = new UserNavigation(visitorId, fullUrl, date);

        driver.withInput(key, asList(nav, nav)).runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.UNPROCESSED_VISITS).getValue());
    }
}
