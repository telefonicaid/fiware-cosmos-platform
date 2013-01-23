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

import java.io.IOException;
import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class KpiCleanerReducerTest {
    private KpiCleanerReducer instance;
    private ReduceDriver<LongWritable, Text, NullWritable, Text> driver;

    @Before
    public void setUp() {
        this.instance = new KpiCleanerReducer();
        this.driver = new ReduceDriver<LongWritable, Text, NullWritable, Text>
                (this.instance);
    }

    @Test
    public void shouldRemoveFirstInput() throws IOException {
        Text a = new Text("a");
        Text b = new Text("b");
        this.driver
                .withInput(new LongWritable(0L), asList(a, b))
                .withOutput(NullWritable.get(), b)
                .run();
    }
}
