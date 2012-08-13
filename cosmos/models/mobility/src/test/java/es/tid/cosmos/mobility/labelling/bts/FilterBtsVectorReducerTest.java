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

package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.google.protobuf.Message;
import static java.util.Arrays.asList;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.BtsUtil;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;

/**
 *
 * @author dmicol
 */
public class FilterBtsVectorReducerTest extends ConfiguredTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>, LongWritable,
            TypedProtobufWritable<Cluster>> driver;
    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                LongWritable, TypedProtobufWritable<Cluster>>(
                        new FilterBtsVectorReducer());
        this.driver.setConfiguration(this.getConf());
    }

    @Test
    public void testNonConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                BtsUtil.create(1, 50000, 2, 3, 6, asList(5L, 6L, 7L)));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final TypedProtobufWritable<Cluster> outValue = new TypedProtobufWritable<Cluster>(
                ClusterUtil.create(1, 2, 0, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                BtsUtil.create(1, 80000, 2, 3, 4, asList(5L, 6L, 7L)));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final TypedProtobufWritable<Cluster> outValue = new TypedProtobufWritable<Cluster>(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
