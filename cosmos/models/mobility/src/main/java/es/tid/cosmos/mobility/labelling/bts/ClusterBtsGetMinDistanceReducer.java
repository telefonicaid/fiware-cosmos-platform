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

package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <Long, Cluster>
 *
 * @author dmicol
 */
class ClusterBtsGetMinDistanceReducer extends Reducer<
        ProtobufWritable<NodeBts>, TypedProtobufWritable<ClusterVector>,
        LongWritable, TypedProtobufWritable<Cluster>> {
    private static List<Cluster> centroids = null;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        if (centroids == null) {
            final MobilityConfiguration conf = new MobilityConfiguration(context.
                    getConfiguration());
            centroids = CentroidsCatalogue.load(new Path(conf.get("centroids")),
                                                conf);
        }
    }

    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<TypedProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        for (TypedProtobufWritable<ClusterVector> value : values) {
            final ClusterVector clusVector = value.get();
            double mindist = Double.POSITIVE_INFINITY;
            Cluster minDistCluster = null;
            for (Cluster cluster : centroids) {
                double dist = 0D;
                for (int nComs = 0; nComs < clusVector.getComsCount(); nComs++) {
                    double ccom = cluster.getCoords().getComs(nComs);
                    double com = clusVector.getComs(nComs);
                    dist += ((ccom - com) * (ccom - com));
                }
                if (dist < mindist) {
                    mindist = dist;
                    minDistCluster = cluster;
                }
            }
            mindist = Math.sqrt(mindist);

            Cluster outputCluster = ClusterUtil.create(
                            minDistCluster.getLabel(),
                            minDistCluster.getLabelgroup(),
                            1,
                            0D,
                            mindist,
                            clusVector);
            context.write(new LongWritable(nodeBts.getBts()),
                          new TypedProtobufWritable<Cluster>(outputCluster));
        }
    }
}
