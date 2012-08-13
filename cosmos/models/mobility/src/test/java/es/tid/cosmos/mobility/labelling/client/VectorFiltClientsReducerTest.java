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

package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.List;

import com.google.protobuf.Message;
import static java.util.Arrays.asList;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientsReducerTest extends ConfiguredTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>, LongWritable,
            TypedProtobufWritable<Cdr>> driver;

    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                LongWritable, TypedProtobufWritable<Cdr>>(
                        new VectorFiltClientsReducer());
        this.driver.setConfiguration(this.getConf());
    }

    @Test
    public void testNoNumberOfCommunications() throws IOException {
        TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                Cdr.getDefaultInstance());
        TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                Cdr.getDefaultInstance());
        List<Pair<LongWritable, TypedProtobufWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(17L), asList(value1, value2))
                .run();
        assertNotNull(res);
        assertEquals(0, res.size());
    }

    @Test(expected=IllegalStateException.class)
    public void testInvalidNumberOfCommunications() throws IOException {
        TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                Int.newBuilder().setValue(56).build());
        TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                Int.newBuilder().setValue(137).build());
        this.driver
                .withInput(new LongWritable(17L), asList(value1, value2))
                .run();
    }
}
