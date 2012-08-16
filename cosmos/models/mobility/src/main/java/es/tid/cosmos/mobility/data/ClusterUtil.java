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

package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;

/**
 *
 * @author sortega
 */
public final class ClusterUtil {

    private ClusterUtil() {}

    public static Cluster create(int label, int labelgroup, int confident,
            double mean, double distance, ClusterVector coords) {
        return Cluster.newBuilder()
                .setLabel(label)
                .setLabelgroup(labelgroup)
                .setConfident(confident)
                .setMean(mean)
                .setDistance(distance)
                .setCoords(coords)
                .build();
    }

    public static ProtobufWritable<Cluster> wrap(Cluster obj) {
        ProtobufWritable<Cluster> wrapper = ProtobufWritable.newInstance(
                Cluster.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Cluster> createAndWrap(int label,
            int labelgroup, int confident, double mean, double distance,
            ClusterVector coords) {
        return wrap(create(label, labelgroup, confident, mean, distance, coords));
    }

    public static String toString(Cluster obj, String separator) {
        String str = obj.getLabel() + separator + obj.getLabelgroup()
                + separator + obj.getConfident() + separator + obj.getMean()
                + separator + obj.getDistance();
        for (double comm : obj.getCoords().getComsList()) {
            str += separator + comm;
        }
        return str;
    }
}