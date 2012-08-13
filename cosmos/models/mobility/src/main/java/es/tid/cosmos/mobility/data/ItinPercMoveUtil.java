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

import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;

/**
 *
 * @author dmicol
 */
public final class ItinPercMoveUtil {

    private ItinPercMoveUtil() {}

    public static ItinPercMove create(int group, int range, double percMoves) {
        return ItinPercMove.newBuilder()
                .setGroup(group)
                .setRange(range)
                .setPercMoves(percMoves)
                .build();
    }

    public static ProtobufWritable<ItinPercMove> wrap(ItinPercMove obj) {
        ProtobufWritable<ItinPercMove> wrapper = ProtobufWritable.newInstance(
                ItinPercMove.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ItinPercMove> createAndWrap(int group,
            int range, double percMoves) {
        return wrap(create(group, range, percMoves));
    }
}
