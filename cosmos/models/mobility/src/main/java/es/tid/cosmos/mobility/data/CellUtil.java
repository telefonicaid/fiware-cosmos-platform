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

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public final class CellUtil {

    private CellUtil() {}

    public static Cell create(long cellId, long bts, int geoLoc1,
                              int geoLoc2, double posX, double posY) {
        return Cell.newBuilder()
                .setCellId(cellId)
                .setBts(bts)
                .setGeoloc1(geoLoc1)
                .setGeoloc2(geoLoc2)
                .setPosx(posX)
                .setPosy(posY)
                .build();
    }

    public static ProtobufWritable<Cell> wrap(Cell obj) {
        ProtobufWritable<Cell> wrapper =
                ProtobufWritable.newInstance(Cell.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Cell> createAndWrap(long cellId,
            long bts, int geoLoc1, int geoLoc2, double posX, double posY) {
        return wrap(create(cellId, bts, geoLoc1, geoLoc2, posX, posY));
    }
}
