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

package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class ParseCdrMapperTest extends ConfiguredTest {
    private MapDriver<LongWritable, Text, LongWritable,
            TypedProtobufWritable<Cdr>> driver;

    @Before
    public void setUp() throws IOException {
        this.driver = new MapDriver<LongWritable, Text, LongWritable,
                TypedProtobufWritable<Cdr>>(new ParseCdrMapper());
        this.driver.setConfiguration(this.getConf());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, TypedProtobufWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(1L),
                           new Text("33F430521676F4|2221436242|"
                                  + "33F430521676F4|0442224173253|2|"
                                  + "01/01/2010|02:00:01|2891|RMITERR"))
                .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        assertEquals(new LongWritable(2221436242L), res.get(0).getFirst());
        TypedProtobufWritable<Cdr> wrappedCdr = res.get(0).getSecond();
        assertNotNull(wrappedCdr.get());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           new Text("33F430521676F4|blah blah|43242"))
                .runTest();
    }

    @Test
    public void testFilterBeforeStartDate() throws Exception {
        this.driver.getConfiguration().set("mob.data_start_date", "02/01/2010");
        List<Pair<LongWritable, TypedProtobufWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(1L),
                        new Text("33F430521676F4|2221436242|"
                                + "33F430521676F4|0442224173253|2|"
                                + "01/01/2010|02:00:01|2891|RMITERR"))
                .run();
        assertEquals(0, res.size());
    }
    @Test
    public void testNotFilterBeforeStartDate() throws Exception {
        this.driver.getConfiguration().set("mob.data_start_date", "02/01/2010");
        List<Pair<LongWritable, TypedProtobufWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(1L),
                        new Text("33F430521676F4|2221436242|"
                                + "33F430521676F4|0442224173253|2|"
                                + "02/01/2010|02:00:01|2891|RMITERR"))
                .run();
        assertEquals(1, res.size());
    }

    @Test
    public void testNotFilterAfterEndDate() throws Exception {
        this.driver.getConfiguration().set("mob.data_end_date", "01/01/2010");
        List<Pair<LongWritable, TypedProtobufWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(1L),
                        new Text("33F430521676F4|2221436242|"
                                + "33F430521676F4|0442224173253|2|"
                                + "01/01/2010|02:00:01|2891|RMITERR"))
                .run();
        assertEquals(1, res.size());
    }

    @Test
    public void testFilterAfterEndDate() throws Exception {
        this.driver.getConfiguration().set("mob.data_end_date", "01/01/2010");
        List<Pair<LongWritable, TypedProtobufWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(1L),
                        new Text("33F430521676F4|2221436242|"
                                + "33F430521676F4|0442224173253|2|"
                                + "02/01/2010|02:00:01|2891|RMITERR"))
                .run();
        assertEquals(0, res.size());
    }
}
