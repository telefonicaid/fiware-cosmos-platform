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

package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class MatrixSpreadNodeMapperTest {
    private MapDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<ItinTime>,
            LongWritable, TypedProtobufWritable<ItinTime>> instance;

    @Before
    public void setUp() {
        this.instance = new MapDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<ItinTime>, LongWritable,
                TypedProtobufWritable<ItinTime>>(new MatrixSpreadNodeMapper());
    }

    @Test
    public void testMap() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L,
                                                                      102L);
        final TypedProtobufWritable<ItinTime> value = new TypedProtobufWritable<ItinTime>(
                ItinTime.getDefaultInstance());
        final LongWritable outKey = new LongWritable(57L);
        this.instance
                .withInput(key, value)
                .withOutput(outKey, value)
                .runTest();
    }
}
