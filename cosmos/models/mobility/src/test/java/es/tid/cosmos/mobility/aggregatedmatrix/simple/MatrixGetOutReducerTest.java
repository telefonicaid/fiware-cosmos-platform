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

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static java.util.Arrays.asList;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class MatrixGetOutReducerTest extends ConfiguredTest {
    private ReduceDriver<ProtobufWritable<TwoInt>,
            TypedProtobufWritable<ClusterVector>, NullWritable, Text> instance;

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<ClusterVector>, NullWritable, Text>(
                        new MatrixGetOutReducer());
        this.instance.setConfiguration(this.getConf());
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L,
                                                                      102L);
        final TypedProtobufWritable<ClusterVector> value = new TypedProtobufWritable<ClusterVector>(
                ClusterVector.getDefaultInstance());
        List<Pair<NullWritable, Text>> results = this.instance
                .withInput(key, asList(value, value))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        assertEquals(NullWritable.get(), results.get(0).getFirst());
        assertTrue(results.get(0).getSecond().toString().startsWith("57|102"));
        assertEquals(NullWritable.get(), results.get(1).getFirst());
        assertTrue(results.get(1).getSecond().toString().startsWith("57|102"));
    }
}
