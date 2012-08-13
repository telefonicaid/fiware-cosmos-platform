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

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 *
 * @author sortega
 */
public class RepbtsAggbybtsReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBtsDay>, TypedProtobufWritable<Int>,
            LongWritable, TypedProtobufWritable<NodeBtsDay>>
            driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBtsDay>,
                TypedProtobufWritable<Int>, LongWritable,
                TypedProtobufWritable<NodeBtsDay>>(new RepbtsAggbybtsReducer());
    }

    @Test
    public void testReduce() throws Exception {
        int node = 123;
        int bts = 456;
        int workday = 1;
        this.driver
                .withInput(NodeBtsDayUtil.createAndWrap(node, bts, workday, 101),
                           asList(TypedProtobufWritable.create(4),
                                  TypedProtobufWritable.create(5)))
                .withOutput(new LongWritable(node), new TypedProtobufWritable<NodeBtsDay>(
                        NodeBtsDayUtil.create(node, bts, workday, 9)))
                .runTest();
    }
}
