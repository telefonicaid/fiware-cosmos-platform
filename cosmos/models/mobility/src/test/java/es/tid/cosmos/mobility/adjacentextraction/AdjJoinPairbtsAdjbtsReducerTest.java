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

package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjJoinPairbtsAdjbtsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<InputIdRecord>,
            LongWritable, TypedProtobufWritable<TwoInt>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<InputIdRecord>, LongWritable,
                TypedProtobufWritable<TwoInt>>(new AdjJoinPairbtsAdjbtsReducer());
    }

    @Test
    public void testWithOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        TwoInt value1 = TwoIntUtil.create(57L, 32L);
        TypedProtobufWritable<InputIdRecord> record1 = new TypedProtobufWritable<InputIdRecord>(
                InputIdRecord.newBuilder()
                             .setInputId(0)
                             .setMessageBytes(value1.toByteString())
                             .build());
        TwoInt value2 = TwoIntUtil.create(157L, 132L);
        TypedProtobufWritable<InputIdRecord> record2 = new TypedProtobufWritable<InputIdRecord>(
                InputIdRecord.newBuilder()
                             .setInputId(1)
                             .setMessageBytes(value2.toByteString())
                             .build());
        final LongWritable outKey = new LongWritable(57L);
        final TypedProtobufWritable<TwoInt> outValue = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(57L, 32L));
        this.driver
                .withInput(key, asList(record1, record2))
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void testNoOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        TwoInt value1 = TwoIntUtil.create(57L, 32L);
        TypedProtobufWritable<InputIdRecord> record1 = new TypedProtobufWritable<InputIdRecord>(
                InputIdRecord.newBuilder()
                             .setInputId(0)
                             .setMessageBytes(value1.toByteString())
                             .build());
        this.driver
                .withInput(key, asList(record1))
                .runTest();
    }
}
