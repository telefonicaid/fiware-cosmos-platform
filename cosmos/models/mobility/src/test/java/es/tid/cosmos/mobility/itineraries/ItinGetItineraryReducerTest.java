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

package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Itinerary;

/**
 *
 * @author dmicol
 */
public class ItinGetItineraryReducerTest extends ConfiguredTest {
    private ReduceDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ClusterVector>, LongWritable,
            TypedProtobufWritable<Itinerary>> instance;

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<ProtobufWritable<ItinRange>,
                TypedProtobufWritable<ClusterVector>, LongWritable,
                TypedProtobufWritable<Itinerary>>(new ItinGetItineraryReducer());
        this.instance.setConfiguration(this.getConf());
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(1L,
                2L, 3, 4, 5);
        ClusterVector.Builder clusterVector = ClusterVector.newBuilder();
        for (int i = 0; i < 168; i++) {
            clusterVector.addComs(i);
        }
        final TypedProtobufWritable<ClusterVector> value =
                new TypedProtobufWritable<ClusterVector>(clusterVector.build());
        List<Pair<LongWritable, TypedProtobufWritable<Itinerary>>> results =
                this.instance
                        .withInput(key, Arrays.asList(value))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
    }
}
