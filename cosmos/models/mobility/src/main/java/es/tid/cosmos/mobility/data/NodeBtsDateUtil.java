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

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;

/**
 *
 * @author ximo
 */
public final class NodeBtsDateUtil {

    private NodeBtsDateUtil() {}

    public static NodeBtsDate create(long userId, long bts,
                                     Date date, int hour) {
        return NodeBtsDate.newBuilder()
                .setUserId(userId)
                .setBts(bts)
                .setDate(date)
                .setHour(hour)
                .build();
    }

    public static ProtobufWritable<NodeBtsDate> wrap(NodeBtsDate obj) {
        ProtobufWritable<NodeBtsDate> wrapper =
                ProtobufWritable.newInstance(NodeBtsDate.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<NodeBtsDate> createAndWrap(long userId,
            long bts, Date date, int hour) {
        return wrap(create(userId, bts, date, hour));
    }
}
