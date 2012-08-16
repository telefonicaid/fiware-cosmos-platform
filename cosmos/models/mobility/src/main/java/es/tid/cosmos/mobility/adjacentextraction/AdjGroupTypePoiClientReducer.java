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

package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, PoiNew>
 * Output: <TwoInt, TwoInt>
 *
 * @author dmicol
 */
class AdjGroupTypePoiClientReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<PoiNew>> values, Context context)
            throws IOException, InterruptedException {
        List<PoiNew> poiNewList = new LinkedList<PoiNew>();
        for (TypedProtobufWritable<PoiNew> value : values) {
            final PoiNew poiNew = value.get();
            poiNewList.add(poiNew);
        }

        for (PoiNew curPoi : poiNewList) {
            for (PoiNew tempPoi : poiNewList) {
                if (curPoi.getId() < tempPoi.getId()) {
                    context.write(TwoIntUtil.createAndWrap(curPoi.getBts(),
                                                           tempPoi.getBts()),
                                  new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(
                                          curPoi.getId(), tempPoi.getId())));
                }
            }
        }
    }
}
