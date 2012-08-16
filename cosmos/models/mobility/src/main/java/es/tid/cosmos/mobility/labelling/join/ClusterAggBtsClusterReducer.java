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

package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 * Input: <Long, Poi|Cluster>
 * Output: <Long, Poi>
 *
 * @author dmicol
 */
class ClusterAggBtsClusterReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, LongWritable, TypedProtobufWritable<Poi>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(values, Poi.class, Cluster.class);
        List<Poi> poiList = dividedLists.get(Poi.class);
        List<Cluster> clusterList = dividedLists.get(Cluster.class);

        for (Poi poi : poiList) {
            for (Cluster cluster : clusterList) {
                Poi.Builder outputPoi = Poi.newBuilder(poi);
                outputPoi.setLabelbts(cluster.getLabel());
                outputPoi.setLabelgroupbts(cluster.getLabelgroup());
                outputPoi.setConfidentbts(cluster.getConfident());
                outputPoi.setDistancebts(cluster.getDistance());
                context.write(new LongWritable(poi.getBts()),
                              new TypedProtobufWritable<Poi>(outputPoi.build()));
            }
        }
    }
}
