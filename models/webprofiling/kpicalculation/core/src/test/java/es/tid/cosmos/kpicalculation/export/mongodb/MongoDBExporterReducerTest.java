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

package es.tid.cosmos.kpicalculation.export.mongodb;

import static java.util.Arrays.asList;
import java.util.List;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 * Use case for ExporterReducer
 *
 * @author dmicol
 */
public class MongoDBExporterReducerTest {
    private MongoDBExporterReducer instance;
    private ReduceDriver<LongWritable, Text, LongWritable, BSONWritable> driver;
    private Configuration conf;

    @Before
    public void setUp() {
        this.instance = new MongoDBExporterReducer();
        this.driver = new ReduceDriver<LongWritable, Text, LongWritable,
                                       BSONWritable>(this.instance);
        this.conf = new Configuration();
        this.conf.setStrings("fields", new String[] { "user", "url"} );
    }

    @Test
    public void testReduce() throws Exception {
        List<Pair<LongWritable, BSONWritable>> results =
                this.driver
                        .withConfiguration(this.conf)
                        .withInput(new LongWritable(4L),
                                   asList(new Text("abc\thttp\t9")))
                .run();
        assertEquals(1, results.size());
        Pair<LongWritable, BSONWritable> result = results.get(0);
        assertEquals("abc",
                     result.getSecond().get("user").toString());
        assertEquals("http",
                     result.getSecond().get("url").toString());
        assertEquals(9L, result.getSecond().get("count"));
    }
}
