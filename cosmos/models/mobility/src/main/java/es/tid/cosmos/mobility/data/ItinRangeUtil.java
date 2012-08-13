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

import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 *
 * @author dmicol
 */
public final class ItinRangeUtil {

    private ItinRangeUtil() {}

    public static ItinRange create(long poiSrc, long poiTgt, int node,
                                   int group, int range) {
        return ItinRange.newBuilder()
                .setPoiSrc(poiSrc)
                .setPoiTgt(poiTgt)
                .setNode(node)
                .setGroup(group)
                .setRange(range)
                .build();
    }

    public static ProtobufWritable<ItinRange> wrap(ItinRange obj) {
        ProtobufWritable<ItinRange> wrapper = ProtobufWritable.newInstance(
                ItinRange.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ItinRange> createAndWrap(long poiSrc,
            long poiTgt, int node, int group, int range) {
        return wrap(create(poiSrc, poiTgt, node, group, range));
    }
}
