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

import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 *
 * @author dmicol
 */
public final class PoiUtil {

    private PoiUtil() {}

    public static Poi create(int id, long node, long bts, int labelnode,
            int labelgroupnode, int confidentnode, double distancenode,
            int labelbts, int labelgroupbts, int confidentbts,
            double distancebts, int labelnodebts, int labelgroupnodebts,
            int confidentnodebts, double distancenodebts, int inoutWeek,
            int inoutWend) {
        return Poi.newBuilder()
                .setId(id)
                .setNode(node)
                .setBts(bts)
                .setLabelnode(labelnode)
                .setLabelgroupnode(labelgroupnode)
                .setConfidentnode(confidentnode)
                .setDistancenode(distancenode)
                .setLabelbts(labelbts)
                .setLabelgroupbts(labelgroupbts)
                .setConfidentbts(confidentbts)
                .setDistancebts(distancebts)
                .setLabelnodebts(labelnodebts)
                .setLabelgroupnodebts(labelgroupnodebts)
                .setConfidentnodebts(confidentnodebts)
                .setDistancenodebts(distancenodebts)
                .setInoutWeek(inoutWeek)
                .setInoutWend(inoutWend)
                .build();
    }

    public static ProtobufWritable<Poi> wrap(Poi obj) {
        ProtobufWritable<Poi> wrapper = ProtobufWritable.newInstance(Poi.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Poi> createAndWrap(int id, long node,
            long bts, int labelnode, int labelgroupnode, int confidentnode,
            double distancenode, int labelbts, int labelgroupbts,
            int confidentbts, double distancebts, int labelnodebts,
            int labelgroupnodebts, int confidentnodebts, double distancenodebts,
            int inoutWeek, int inoutWend) {
        return wrap(create(id, node, bts, labelnode, labelgroupnode,
                confidentnode, distancenode, labelbts, labelgroupbts,
                confidentbts, distancebts, labelnodebts, labelgroupnodebts,
                confidentnodebts, distancenodebts, inoutWeek, inoutWend));
    }

    public static String toString(Poi obj, String separator) {
        return (obj.getId() + separator + obj.getNode() + separator +
                obj.getBts() + separator + obj.getLabelnode() + separator +
                obj.getLabelgroupnode() + separator + obj.getConfidentnode() +
                separator + obj.getDistancenode() + separator +
                obj.getLabelbts() + separator + obj.getLabelgroupbts() +
                separator + obj.getConfidentbts() + separator +
                obj.getDistancebts() + separator + obj.getLabelnodebts() +
                separator + obj.getLabelgroupnodebts() + separator +
                obj.getConfidentnodebts() + separator +
                obj.getDistancenodebts() + separator + obj.getInoutWeek() +
                separator + obj.getInoutWend());
    }
}
