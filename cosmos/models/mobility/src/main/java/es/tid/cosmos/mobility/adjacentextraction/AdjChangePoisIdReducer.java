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
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi|PoiNew>
 * Output: <TwoInt, Poi>
 *
 * @author dmicol
 */
class AdjChangePoisIdReducer extends Reducer<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Poi>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> typeLists = TypedProtobufWritable.groupByClass(
                values, Poi.class, PoiNew.class);
        final List<Poi> poiList = typeLists.get(Poi.class);
        final List<PoiNew> poiNewList = typeLists.get(PoiNew.class);
        if (poiList.size() != 1 || poiNewList.size() != 1) {
            throw new IllegalArgumentException("Invalid number of input data");
        }
        final Poi poi = poiList.get(0);
        final PoiNew poiNew = poiNewList.get(0);
        Poi.Builder outputPoiBuilder = Poi.newBuilder(poi);
        outputPoiBuilder.setId(poiNew.getId());
        context.write(key,
                      new TypedProtobufWritable<Poi>(outputPoiBuilder.build()));
    }
}
