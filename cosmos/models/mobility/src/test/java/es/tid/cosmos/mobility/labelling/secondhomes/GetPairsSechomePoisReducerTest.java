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

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static java.util.Arrays.asList;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.PoiPosUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class GetPairsSechomePoisReducerTest extends ConfiguredTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<PoiPos>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Int64>> driver;

    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<PoiPos>,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<Int64>>(
                        new GetPairsSechomePoisReducer());
        this.driver.setConfiguration(this.getConf());
    }

    @Test
    public void shouldProduceOutput() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<PoiPos> value1 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(1, 2, 3, 4, 5, 1, 7, 8, 9, 10, 11));
        final TypedProtobufWritable<PoiPos> value2 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(10, 20, 3, 40000.0D, 40000.0D, 0, 70, 80, 90,
                                  100, 110));
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 20);
        final TypedProtobufWritable<Int64> outValue = TypedProtobufWritable.create(1L);
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputDueToDistance() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<PoiPos> value1 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(1, 2, 3, 10000.0D, 10000.0D, 1, 7, 8, 9, 10,
                                  11));
        final TypedProtobufWritable<PoiPos> value2 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(10, 20, 6, 40000.0D, 40000.0D, 0, 70, 80, 90,
                                  100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputDueToDistanceOneBeingZero() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<PoiPos> value1 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(1, 2, 3, 4, 5, 1, 7, 8, 9, 10, 11));
        final TypedProtobufWritable<PoiPos> value2 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(10, 20, 6, 4000.0D, 4000.0D, 0, 70, 80, 90,
                                  100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputDueToLabelIdsInFirstValue() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<PoiPos> value1 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11));
        final TypedProtobufWritable<PoiPos> value2 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(10, 20, 6, 40000.0D, 40000.0D, 0, 70, 80,
                                    90, 100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputDueToLabelIdsInSecondValue() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<PoiPos> value1 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(1, 2, 3, 4, 5, 1, 7, 8, 9, 10, 11));
        final TypedProtobufWritable<PoiPos> value2 = new TypedProtobufWritable<PoiPos>(
                PoiPosUtil.create(10, 20, 30, 40000.0D, 40000.0D, 0, 70, 80,
                                    90, 100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }
}
