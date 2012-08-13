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

package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.MatrixRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;

/**
 * Input: <MatrixRange, Double>
 * Output: <MatrixRange, ItinPercMove>
 *
 * @author dmicol
 */
class MatrixCountRangesReducer extends Reducer<
        ProtobufWritable<MatrixRange>, TypedProtobufWritable<Float64>,
        ProtobufWritable<MatrixRange>, TypedProtobufWritable<ItinPercMove>> {
    @Override
    protected void reduce(ProtobufWritable<MatrixRange> key,
            Iterable<TypedProtobufWritable<Float64>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(MatrixRange.class);
        final MatrixRange moveRange = key.get();
        double numMoves = 0.0D;
        for (TypedProtobufWritable<Float64> value : values) {
            numMoves += value.get().getValue();
        }
        ProtobufWritable<MatrixRange> range = MatrixRangeUtil.createAndWrap(
                moveRange.getNode(), moveRange.getPoiSrc(),
                moveRange.getPoiTgt(), 0, 0);
        TypedProtobufWritable<ItinPercMove> distMoves = new TypedProtobufWritable<ItinPercMove>(
                ItinPercMoveUtil.create(moveRange.getGroup(),
                                        moveRange.getRange(), numMoves));
        context.write(range, distMoves);
    }
}
