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
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 *
 * @author dmicol
 */
public class MatrixSpreadDistMovesByPairMapperTest {
    private MapDriver<ProtobufWritable<ItinRange>, TypedProtobufWritable<Float64>,
            ProtobufWritable<ItinRange>, TypedProtobufWritable<Float64>> instance;

    @Before
    public void setUp() {
        this.instance = new MapDriver<ProtobufWritable<ItinRange>,
                TypedProtobufWritable<Float64>, ProtobufWritable<ItinRange>,
                TypedProtobufWritable<Float64>>(
                        new MatrixSpreadDistMovesByPairMapper());
    }

    @Test
    public void testMap() {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(
                10, 20, 30, 40, 50);
        final TypedProtobufWritable<Float64> value = TypedProtobufWritable.create(57.0D);
        final ProtobufWritable<ItinRange> outKey = ItinRangeUtil.createAndWrap(
                10, 20, 200, 40, 50);
        this.instance
                .withInput(key, value)
                .withOutput(outKey, value)
                .runTest();
    }
}
