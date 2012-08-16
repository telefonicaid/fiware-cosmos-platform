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

package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;

/**
 *
 * @author dmicol
 */
public class RepbtsJoinDistCommsReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>, LongWritable,
            TypedProtobufWritable<BtsCounter>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                LongWritable, TypedProtobufWritable<BtsCounter>>(
                        new RepbtsJoinDistCommsReducer());
    }

    @Test
    public void testReduce() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> nodeBtsDay1 = new TypedProtobufWritable<Message>(
                NodeBtsDayUtil.create(1L, 2L, 3, 4));
        final TypedProtobufWritable<Message> nodeBtsDay2 = new TypedProtobufWritable<Message>(
                NodeBtsDayUtil.create(5L, 6L, 7, 8));
        final TypedProtobufWritable<Message> int1 = new TypedProtobufWritable<Message>(
                Int.newBuilder().setValue(37).build());
        final TypedProtobufWritable<Message> int2 = new TypedProtobufWritable<Message>(
                Int.newBuilder().setValue(132).build());
        final TypedProtobufWritable<BtsCounter> output1 = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(2L, 0, 4, 10));
        final TypedProtobufWritable<BtsCounter> output2 = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(6L, 0, 8, 21));
        final TypedProtobufWritable<BtsCounter> output3 = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(2L, 0, 4, 3));
        final TypedProtobufWritable<BtsCounter> output4 = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(6L, 0, 8, 6));
        this.driver
                .withInput(key, asList(int1, nodeBtsDay1, nodeBtsDay2, int2))
                .withOutput(key, output1)
                .withOutput(key, output2)
                .withOutput(key, output3)
                .withOutput(key, output4)
                .runTest();
    }
}
