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

package es.tid.cosmos.mobility.activitydensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class PopdenSpreadArrayReducerTest {

    private ReduceDriver<LongWritable, TypedProtobufWritable<NodeMxCounter>,
            ProtobufWritable<BtsProfile>, TypedProtobufWritable<Int>> instance;

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                TypedProtobufWritable<NodeMxCounter>, ProtobufWritable<BtsProfile>,
                TypedProtobufWritable<Int>>(new PopdenSpreadArrayReducer());
    }

    @Test
    public void testReduce() {
        final BtsCounter counter1 = BtsCounterUtil.create(1L, 2, 3, 4);
        final BtsCounter counter2 = BtsCounterUtil.create(5L, 6, 7, 8);
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<NodeMxCounter> value =
                new TypedProtobufWritable<NodeMxCounter>(
                NodeMxCounterUtil.create(counter1, counter2));
        final ProtobufWritable<BtsProfile> outKey1 =
                BtsProfileUtil.createAndWrap(1L, 0, 2, 3);
        final TypedProtobufWritable<Int> outValue1 =
                TypedProtobufWritable.create(4);
        final ProtobufWritable<BtsProfile> outKey2 =
                BtsProfileUtil.createAndWrap(5L, 0, 6, 7);
        final TypedProtobufWritable<Int> outValue2 =
                TypedProtobufWritable.create(8);
        this.instance
                .withInput(key, Arrays.asList(value))
                .withOutput(outKey1, outValue1)
                .withOutput(outKey2, outValue2)
                .runTest();
    }
}
