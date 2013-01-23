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

package es.tid.cosmos.kpicalculation;

import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import static org.apache.hadoop.mrunit.testutil.ExtendedAssert.assertListEquals;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.CompositeKey;
import es.tid.cosmos.base.mapreduce.SingleKey;

/**
 * Test cases for the KpiCounterReducer class.
 */

public class KpiCounterReducerTest {
    private KpiCounterReducer reducer;
    private ReduceDriver<CompositeKey, IntWritable, Text, IntWritable> driver;

    @Before
    public void setUp() {
        this.reducer = new KpiCounterReducer();
        this.driver = new ReduceDriver<CompositeKey, IntWritable, Text,
                                       IntWritable>(this.reducer);
        this.driver.getConfiguration().setStrings("kpi.aggregation.fields",
                                                  "protocol,urlDomain");
        this.driver.getConfiguration().setBoolean("kpi.aggregation.hashmap",
                                                  true);
    }

    @Test
    public void testCounterReducer() throws Exception {
        SingleKey key = new SingleKey();
        key.setKey("http\ttid.es");

        List<IntWritable> values = new ArrayList<IntWritable>();
        values.add(new IntWritable(1));
        values.add(new IntWritable(3));
        values.add(new IntWritable(4));

        List<Pair<Text, IntWritable>> out;
        out = this.driver.withInputKey(key).withInputValues(values).run();

        List<Pair<Text, IntWritable>> expected =
                new ArrayList<Pair<Text,IntWritable>>();
        expected.add(new Pair<Text, IntWritable>(new Text("http\ttid.es"),
                new IntWritable(8)));

        assertListEquals(expected, out);
    }
}
