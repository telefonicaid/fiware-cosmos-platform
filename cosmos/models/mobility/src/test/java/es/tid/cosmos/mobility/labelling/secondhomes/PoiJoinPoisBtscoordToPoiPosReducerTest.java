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

package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiPosReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>, LongWritable,
            TypedProtobufWritable<PoiPos>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                LongWritable, TypedProtobufWritable<PoiPos>>(
                        new PoiJoinPoisBtscoordToPoiPosReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(1L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                CellUtil.create(1, 2, 3, 4, 5, 6));
        final LongWritable outKey = new LongWritable(2L);
        final TypedProtobufWritable<PoiPos> outValue = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(2, 3, 13, 5, 6, 16, 17, -1, -1, -1, -1));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
