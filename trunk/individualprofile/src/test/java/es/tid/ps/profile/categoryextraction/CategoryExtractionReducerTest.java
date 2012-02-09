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

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.dictionary.Categorization;
import es.tid.ps.profile.dictionary.CategorizationResult;
import org.apache.avro.mapred.AvroKey;
import org.apache.avro.mapred.AvroValue;
import org.apache.avro.mapred.AvroWrapper;
import org.apache.avro.mapred.Pair;
import org.apache.avro.mapreduce.AvroSerialization;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.Calendar;
import java.util.Collection;

import static java.util.Arrays.asList;
import static org.junit.Assert.assertEquals;

/**
 * Test case for CategoryExtractionReducer
 *
 * @author sortega@tid.es
 */
public class CategoryExtractionReducerTest {
    private CategoryExtractionReducer instance;
    private ReduceDriver<AvroKey<BinaryKey>, AvroValue<UserNavigation>,
                AvroWrapper<Pair<BinaryKey, CategoryInformation>>, NullWritable> driver;

    @Before
    public void setUp() throws Exception {
        this.instance = new CategoryExtractionReducer() {
            @Override
            protected void setupDictionary(Configuration unused) throws IOException {
                // Avoid loading the real dictionary
            }

            @Override
            protected Categorization categorize(String url) {
                Categorization categorization = new Categorization();
                if (url.equals("http://www.marca.es/basket")) {
                    categorization.setResult(CategorizationResult.KNOWN_URL);
                    categorization.setCategories(
                            new String[] { "SPORTS", "NEWS" });
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

        this.driver = new ReduceDriver<AvroKey<BinaryKey>, AvroValue<UserNavigation>,
                AvroWrapper<Pair<BinaryKey, CategoryInformation>>, NullWritable>();
        addAvroSerializer(this.driver.getConfiguration());
        this.driver.setReducer(instance);
    }

    private void addAvroSerializer(Configuration conf) {
        Collection<String> serializations = conf.getStringCollection("io.serializations");
        if (!serializations.contains(AvroSerialization.class.getName())) {
            serializations.add(AvroSerialization.class.getName());
            conf.setStrings("io.serializations", serializations.toArray(new String[0]));
        }
        conf.set("avro.map.output.schema", new
                Pair<BinaryKey, CategoryInformation>(new BinaryKey(), new CategoryInformation()).getSchema().toString());
    }

    @Test
    public void testKnownUrl() throws Exception {
        String visitorId = "CA003B";
        String fullUrl = "http://www.marca.es/basket";
        Calendar date = Calendar.getInstance();
        BinaryKey key = new BinaryKey(visitorId, date.toString());
        UserNavigation nav = new UserNavigation();
        UserNavigationFactory.set(nav, visitorId, fullUrl, date);

        CategoryInformation expectedCategoryInformation = new CategoryInformation();
        CategoryInformationFactory.set(expectedCategoryInformation, visitorId, fullUrl, date.toString(),
                        2, new String[] {"SPORTS", "NEWS"});

        driver.withInput(new AvroKey<BinaryKey>(key),
                asList(new AvroValue<UserNavigation>(nav), new AvroValue<UserNavigation>(nav)))
              .withOutput(new AvroWrapper<Pair<BinaryKey, CategoryInformation>>(
                      new Pair<BinaryKey, CategoryInformation>(key, expectedCategoryInformation)),
                      NullWritable.get())
              .runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.KNOWN_VISITS).getValue());
    }

    /*
    @Test
    public void testUnknownUrl() throws Exception {
        String visitorId = "CA003C";
        String fullURL = "http://www.mutxamel.org";
        BinaryKey key = new BinaryKey(visitorId, "02/01/2012");
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
        BinaryKey key = new BinaryKey(visitorId, date.toString());
        UserNavigation nav = new UserNavigation();
        UserNavigationFactory.set(nav, visitorId, fullUrl, date);

        driver.withInput(key, asList(nav, nav)).runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.IRRELEVANT_VISITS).getValue());
    }
            
    @Test
    public void testGenericFailure() throws Exception {
        String visitorId = "CA003E";
        String fullUrl = "";
        Calendar date = Calendar.getInstance();
        BinaryKey key = new BinaryKey(visitorId, date.toString());
        UserNavigation nav = new UserNavigation();
        UserNavigationFactory.set(nav, visitorId, fullUrl, date);

        driver.withInput(key, asList(nav, nav)).runTest();
        assertEquals(2l, driver.getCounters().findCounter(
                CategoryExtractionCounter.UNPROCESSED_VISITS).getValue());
    }*/
}
