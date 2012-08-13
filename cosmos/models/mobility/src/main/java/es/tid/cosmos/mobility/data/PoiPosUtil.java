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

import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;

/**
 *
 * @author dmicol
 */
public final class PoiPosUtil {

    private PoiPosUtil() {}

    public static PoiPos create(long node, long bts, int label, double posx,
            double posy, int inoutWeek, int inoutWend, double radiusWeek,
            double distCMWeek, double radiusWend, double distCMWend) {
        return PoiPos.newBuilder()
                .setNode(node)
                .setBts(bts)
                .setLabel(label)
                .setPosx(posx)
                .setPosy(posy)
                .setInoutWeek(inoutWeek)
                .setInoutWend(inoutWend)
                .setRadiusWeek(radiusWeek)
                .setDistCMWeek(distCMWeek)
                .setRadiusWend(radiusWend)
                .setDistCMWend(distCMWend)
                .build();
    }

    public static ProtobufWritable<PoiPos> wrap(PoiPos obj) {
        ProtobufWritable<PoiPos> wrapper = ProtobufWritable.newInstance(
                PoiPos.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<PoiPos> createAndWrap(long node, long bts,
            int label, double posx, double posy, int inoutWeek, int inoutWend,
            double radiusWeek, double distCMWeek, double radiusWend,
            double distCMWend) {
        return wrap(create(node, bts, label, posx, posy, inoutWeek, inoutWend,
                radiusWeek, distCMWeek, radiusWend, distCMWend));
    }
}
