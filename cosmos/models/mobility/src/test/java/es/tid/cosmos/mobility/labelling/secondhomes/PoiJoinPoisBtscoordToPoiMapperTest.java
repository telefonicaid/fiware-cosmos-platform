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

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiMapperTest {
    private MapDriver<LongWritable, TypedProtobufWritable<Poi>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<Poi>,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>>(
                        new PoiJoinPoisBtscoordToPoiMapper());
    }

    @Test
    public void testReduce() {
        final TypedProtobufWritable<Poi> value = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                               11, 12, 13, 14, 15, 16, 17));
        this.driver
                .withInput(new LongWritable(57L), value)
                .withOutput(TwoIntUtil.createAndWrap(2, 3), value)
                .runTest();
    }
}
