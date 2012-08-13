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

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Replaces the value by two copies of the key.
 *
 * Input:  <LongWritable, Poi>
 * Output: <LongWritable, TwoInt>
 *
 * @author dmicol, sortega
 */
class AdjAddUniqueIdPoiToTwoIntMapper extends Mapper<LongWritable,
        TypedProtobufWritable<Poi>, LongWritable, TypedProtobufWritable<TwoInt>> {

    @Override
    protected void map(LongWritable uuidKey, TypedProtobufWritable<Poi> value,
            Context context) throws IOException, InterruptedException {
        long uuid = uuidKey.get();
        TwoInt poiPoiPair = TwoIntUtil.create(uuid, uuid);
        context.write(uuidKey, new TypedProtobufWritable<TwoInt>(poiPoiPair));
    }
}
