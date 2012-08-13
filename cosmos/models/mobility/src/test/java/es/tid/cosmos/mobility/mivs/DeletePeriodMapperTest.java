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

package es.tid.cosmos.mobility.mivs;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TelMonthUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;

/**
 *
 * @author dmicol
 */
public class DeletePeriodMapperTest {
    private MapDriver<ProtobufWritable<TelMonth>, TypedProtobufWritable<Cell>,
        ProtobufWritable<TelMonth>, TypedProtobufWritable<Cell>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TelMonth>,
                TypedProtobufWritable<Cell>, ProtobufWritable<TelMonth>,
                TypedProtobufWritable<Cell>>(new DeletePeriodMapper());
    }

    @Test
    public void testMap() throws Exception {
        ProtobufWritable<TelMonth> inputKey = TelMonthUtil.createAndWrap(
                1232121L, 7, true);
        ProtobufWritable<TelMonth> outputkey = TelMonthUtil.createAndWrap(
                1232121L, 1, true);
        TypedProtobufWritable<Cell> value = new TypedProtobufWritable<Cell>(
                CellUtil.create(1L, 2L, 3, 4, 5D, 6D));
        this.driver
                .withInput(inputKey, value)
                .withOutput(outputkey, value)
                .runTest();
    }
}
