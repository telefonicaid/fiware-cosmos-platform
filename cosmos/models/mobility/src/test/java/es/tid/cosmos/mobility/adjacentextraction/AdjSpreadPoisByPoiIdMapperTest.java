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

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadPoisByPoiIdMapperTest {
    private MapDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>,
            LongWritable, TypedProtobufWritable<PoiNew>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<PoiNew>, LongWritable,
                TypedProtobufWritable<PoiNew>>(new AdjSpreadPoisByPoiIdMapper());
    }

    @Test
    public void testMap() {
        final TypedProtobufWritable<PoiNew> poiNew = new TypedProtobufWritable<PoiNew>(
                PoiNewUtil.create(1, 2L, 3L, 4, 0));
        this.driver
                .withInput(TwoIntUtil.createAndWrap(5L, 6L), poiNew)
                .withOutput(new LongWritable(1L), poiNew)
                .runTest();
    }
}
