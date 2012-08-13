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

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, PoiNew>
 * Output: <Long, PoiNew>
 *
 * @author dmicol
 */
class AdjSpreadPoisByPoiIdMapper extends Mapper<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>, LongWritable,
        TypedProtobufWritable<PoiNew>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            TypedProtobufWritable<PoiNew> value, Context context)
            throws IOException, InterruptedException {
        final PoiNew poi = value.get();
        context.write(new LongWritable(poi.getId()), value);
    }
}
