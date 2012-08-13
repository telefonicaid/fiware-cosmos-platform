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

package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobViMobVarsUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;

/**
 * Input: <Long, MobVars>
 * Output: <Long, MobViMobVars>
 *
 * @author logc
 */
class FusionTotalVarsReducer extends Reducer<LongWritable,
        TypedProtobufWritable<MobVars>, LongWritable, TypedProtobufWritable<MobViMobVars>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<MobVars>> values, Context context)
            throws IOException, InterruptedException {
        List<MobVars> allAreas = new ArrayList<MobVars>();
        for (TypedProtobufWritable<MobVars> value: values) {
            allAreas.add(value.get());
        }
        context.write(key, new TypedProtobufWritable<MobViMobVars>(
                MobViMobVarsUtil.create(allAreas)));
    }
}
