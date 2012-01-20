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
package es.tid.ps.dynamicprofile.categoryextraction;

import org.apache.hadoop.io.NullWritable;
import java.io.IOException;
import org.apache.hadoop.mapreduce.Reducer.Context;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;
import static java.util.Arrays.*;

/**
 * Test case for CategoryExtractionReducer
 *
 * @author sortega@tid.es
 */
public class CategoryExtractionReducerTest {
    private CategoryExtractionReducer instance;
    private ReduceDriver<CompositeKey, NullWritable, CompositeKey,
            CategoryInformation> driver;

    @Before
    public void setUp() {
        instance = new CategoryExtractionReducer() {
            @Override
            public void setup(Context context) throws IOException {
                // Preventing dictionay initialization
            }

            @Override
            protected String getCategories(String url) {
                return "SPORTS/NEWS";
            }
        };
        driver = new ReduceDriver<CompositeKey, NullWritable, CompositeKey,
                CategoryInformation>(instance);
    }

    @Test
    public void testReduce() throws Exception {
        String visitorId = "CA003B";
        String fullURL = "http://www.marca.es/basket";
        CompositeKey key = new CompositeKey(visitorId, fullURL);

        CategoryInformation expectedCategoryInformation =
                new CategoryInformation(visitorId, fullURL, 2,
                        new String[] {"SPORTS", "NEWS"});

        driver.withInput(key, asList(NullWritable.get(), NullWritable.get()))
              .withOutput(key, expectedCategoryInformation)
              .runTest();
    }
}
