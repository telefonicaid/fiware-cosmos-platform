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

package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;

/**
 * Input: <Long, PoiPos|MobViMobVars>
 * Output: <Long, PoiPos>
 *
 * @author dmicol
 */
class PoiJoinPoisViToPoiPosReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, LongWritable, TypedProtobufWritable<PoiPos>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, PoiPos.class, MobViMobVars.class);
        List<PoiPos> poiPosList = dividedLists.get(PoiPos.class);
        List<MobViMobVars> mobVIVarsList = dividedLists.get(MobViMobVars.class);

        for (PoiPos poiPos : poiPosList) {
            for (MobViMobVars indvars : mobVIVarsList) {
                PoiPos.Builder outputPoiPos = PoiPos.newBuilder(poiPos);
                for (MobVars vars : indvars.getVarsList()) {
                    double distx = poiPos.getPosx() - vars.getMasscenterUtmx();
                    double disty = poiPos.getPosy() - vars.getMasscenterUtmy();
                    double dist = Math.sqrt(distx * distx + disty * disty);
                    if (vars.getWorkingday()) {
                        // Individual variables Monday - Friday
                        outputPoiPos.setInoutWeek(
                                dist <= vars.getRadius() ? 1 : 0);
                    } else {
                        // Individual variables Saturday - Sunday
                        outputPoiPos.setInoutWend(
                                dist <= vars.getRadius() ? 1 : 0);
                    }
                    outputPoiPos.setRadiusWeek(vars.getRadius());
                    outputPoiPos.setDistCMWeek(dist);
                }
                context.write(key,
                              new TypedProtobufWritable<PoiPos>(outputPoiPos.build()));
            }
        }
    }
}
