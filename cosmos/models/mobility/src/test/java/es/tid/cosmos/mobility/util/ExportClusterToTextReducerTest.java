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

package es.tid.cosmos.mobility.util;

import java.io.IOException;

import static java.util.Arrays.asList;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;

/**
 *
 * @author dmicol
 */
public class ExportClusterToTextReducerTest extends ConfiguredTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Cluster>, NullWritable,
            Text> driver;

    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Cluster>,
                NullWritable, Text>(new ExportClusterToTextReducer());
        this.driver.setConfiguration(this.getConf());
    }

    @Test
    public void testSetInputId() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final ClusterVector clusterVector = ClusterVector.newBuilder()
                .addAllComs(asList(6D, 7D, 8D)).build();
        final TypedProtobufWritable<Cluster> value = new TypedProtobufWritable<Cluster>(
                ClusterUtil.create(1, 2, 3, 4, 5, clusterVector));
        final Text outValue = new Text("57|1|2|3|4.0|5.0|6.0|7.0|8.0");
        this.driver
                .withInput(key, asList(value))
                .withOutput(NullWritable.get(), outValue)
                .runTest();
    }
}
