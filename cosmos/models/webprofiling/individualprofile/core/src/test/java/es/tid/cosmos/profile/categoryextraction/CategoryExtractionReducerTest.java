/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
package es.tid.cosmos.profile.categoryextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.Calendar;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer.Context;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.base.mapreduce.TernaryKey;
import es.tid.cosmos.profile.dictionary.Categorization;
import es.tid.cosmos.profile.dictionary.CategorizationResult;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryInformation;

/**
 * Test case for CategoryExtractionReducer
 *
 * @author sortega@tid.es
 */
public class CategoryExtractionReducerTest {
    private final LongWritable one = new LongWritable(1L);
    private CategoryExtractionReducer instance;
    private ReduceDriver<TernaryKey, LongWritable, BinaryKey,
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
        this.driver = new ReduceDriver<TernaryKey, LongWritable,
                BinaryKey, ProtobufWritable<CategoryInformation>>(this.instance);
    }

    @Test
    public void testKnownUrl() throws Exception {
        String visitorId = "CA003B";
        String fullUrl = "http://www.marca.es/basket";
        Calendar date = Calendar.getInstance();
        TernaryKey inKey = new TernaryKey(visitorId, date.toString(), fullUrl);
        BinaryKey outKey = new BinaryKey(visitorId, date.toString());

        List<Pair<BinaryKey, ProtobufWritable<CategoryInformation>>> results =
                driver.withInput(inKey, asList(one, one)).run();

        assertEquals(1, results.size());
        final Pair<BinaryKey, ProtobufWritable<CategoryInformation>> result =
                results.get(0);
        assertEquals(outKey, result.getFirst());

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
        TernaryKey inKey = new TernaryKey(visitorId, date.toString(), fullUrl);

        driver.withInput(inKey, asList(one, one, one))
                .runTest();
        assertEquals(3L, driver.getCounters().findCounter(
                CategoryExtractionCounter.UNKNOWN_VISITS).getValue());
    }

    @Test
    public void testIrrelevantUrl() throws Exception {
        String visitorId = "CA003D";
        String fullUrl = "http://www.realmadrid.com";
        Calendar date = Calendar.getInstance();
        TernaryKey inKey = new TernaryKey(visitorId, date.toString(), fullUrl);

        driver.withInput(inKey, asList(one))
                .runTest();
        assertEquals(1L, driver.getCounters().findCounter(
                CategoryExtractionCounter.IRRELEVANT_VISITS).getValue());
    }

    @Test
    public void testGenericFailure() throws Exception {
        String visitorId = "CA003E";
        String fullUrl = "";
        Calendar date = Calendar.getInstance();
        TernaryKey inKey = new TernaryKey(visitorId, date.toString(), fullUrl);

        driver.withInput(inKey, asList(one, one))
                .runTest();
        assertEquals(2L, driver.getCounters().findCounter(
                CategoryExtractionCounter.UNPROCESSED_VISITS).getValue());
    }
}
