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

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 * Input: <Long, Cluster>
 * Output: <Null, Text>
 *
 * @author dmicol
 */
public class ExportClusterToTextReducer extends Reducer<
        LongWritable, TypedProtobufWritable<Cluster>, NullWritable, Text> {
    private String separator;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<Cluster> value : values) {
            final Cluster cluster = value.get();
            context.write(NullWritable.get(),
                          new Text(key + this.separator
                                   + ClusterUtil.toString(cluster,
                                                          this.separator)));
        }
    }
}
