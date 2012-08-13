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

package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, BtsCounter>
 * Output: <TwoInt, ClusterVector>
 *
 * @author dmicol
 */
public class PopdenCreateVectorReducer extends Reducer<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<BtsCounter>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<ClusterVector>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        ClusterVector.Builder comsVector = ClusterVector.newBuilder();
        for (int i = 0; i < 168; i++) {
            comsVector.addComs(0.0D);
        }
        for (TypedProtobufWritable<BtsCounter> value : values) {
            final BtsCounter counter = value.get();
            int wday = counter.getWeekday();
            int pos = wday > 0 ? wday - 1 : 6;	// Sunday: 0
            pos *= 24;
            pos += counter.getRange();
            double norm = counter.getCount();
            if (counter.getWeekday() == 4) {
                norm /= 25.0;
            } else {
                norm /= 26.0;
            }
            comsVector.setComs(pos, norm);
        }
        context.write(key, new TypedProtobufWritable<ClusterVector>(
                comsVector.build()));
    }
}
