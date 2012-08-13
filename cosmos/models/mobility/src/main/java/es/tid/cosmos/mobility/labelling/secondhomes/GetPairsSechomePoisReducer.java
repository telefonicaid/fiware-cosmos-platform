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
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, PoiPos>
 * Output: <TwoInt, Long>
 *
 * @author dmicol
 */
class GetPairsSechomePoisReducer extends Reducer<LongWritable,
        TypedProtobufWritable<PoiPos>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Int64>> {
    private int homeLabelgroupId;
    private double minDistSecondHome;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.homeLabelgroupId = conf.getPoiHomeLabelgroupId();
        this.minDistSecondHome = conf.getPoiMinDistSecondHome();
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<PoiPos>> values, Context context)
            throws IOException, InterruptedException {
        List<PoiPos> poiPosList = new LinkedList<PoiPos>();
        for (TypedProtobufWritable<PoiPos> value : values) {
            poiPosList.add(value.get());
        }

        for (PoiPos poiIn : poiPosList) {
            if (poiIn.getInoutWeek() == 1 &&
                    poiIn.getLabel() == this.homeLabelgroupId) {
                for (PoiPos poiOut : poiPosList) {
                    if (poiOut.getInoutWeek() == 0 &&
                            poiOut.getLabel() == this.homeLabelgroupId) {
                        double distx = poiIn.getPosx() - poiOut.getPosx();
                        double disty = poiIn.getPosy() - poiOut.getPosy();
                        double dist = Math.sqrt(distx * distx + disty * disty);
                        if (dist >= this.minDistSecondHome) {
                            context.write(
                                    TwoIntUtil.createAndWrap(poiIn.getBts(),
                                                             poiOut.getBts()),
                                    TypedProtobufWritable.create(poiIn.getNode()));
                        }
                    }
                }
            }
        }
    }
}
