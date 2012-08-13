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

package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToClusterReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Cluster>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Cluster>>(
                        new ClusterAggPotPoiPoisToClusterReducer());
    }

    @Test
    public void shoulChangeConfidence() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                ClusterUtil.create(5, 6, 1, 7, 8,
                                   ClusterVector.getDefaultInstance()));
        // This value is what makes the reducer change the confidence
        final TypedProtobufWritable<Message> value3 = new TypedProtobufWritable<Message>(
                Null.getDefaultInstance());
        final TypedProtobufWritable<Cluster> outValue = new TypedProtobufWritable<Cluster>(
                ClusterUtil.create(1, 2, 1, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void shouldNotChangeConfidence() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                ClusterUtil.create(5, 6, 1, 7, 8,
                                   ClusterVector.getDefaultInstance()));
        final TypedProtobufWritable<Cluster> outValue = new TypedProtobufWritable<Cluster>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
