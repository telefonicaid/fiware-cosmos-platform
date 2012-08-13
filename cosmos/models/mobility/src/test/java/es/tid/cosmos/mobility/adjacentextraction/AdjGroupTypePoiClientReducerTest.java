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

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
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
public class AdjGroupTypePoiClientReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<TwoInt>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<PoiNew>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<TwoInt>>(new AdjGroupTypePoiClientReducer());
    }

    @Test
    public void testSomeMethod() throws IOException {
        PoiNew pn1 = PoiNewUtil.create(1, 2L, 3L, 4, 1);
        PoiNew pn2 = PoiNewUtil.create(5, 6L, 7L, 8, 0);
        List<Pair<ProtobufWritable<TwoInt>, TypedProtobufWritable<TwoInt>>> res =
                this.driver
                        .withInput(TwoIntUtil.createAndWrap(1, 2),
                                   asList(new TypedProtobufWritable<PoiNew>(pn1),
                                          new TypedProtobufWritable<PoiNew>(pn2)))
                .run();
        assertEquals(1, res.size());
        final ProtobufWritable<TwoInt> keyWrapper = res.get(0).getFirst();
        final TypedProtobufWritable<TwoInt> valueWrapper = res.get(0).getSecond();
        keyWrapper.setConverter(TwoInt.class);
        final TwoInt key = keyWrapper.get();
        final TwoInt value = valueWrapper.get();
        assertEquals(3L, key.getNum1());
        assertEquals(7L, key.getNum2());
        assertEquals(1L, value.getNum1());
        assertEquals(5L, value.getNum2());
    }
}
