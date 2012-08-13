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

package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <BtsCounter, Null>
 * Output: <NodeBts, TwoInt>
 *
 * @author dmicol
 */
class VectorSumComsBtsReducer extends Reducer<
        ProtobufWritable<BtsCounter>, TypedProtobufWritable<Null>,
        ProtobufWritable<NodeBts>, TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<BtsCounter> key,
            Iterable<TypedProtobufWritable<Null>> values, Context context)
            throws IOException,InterruptedException {
        int valueCount = 0;
        for (TypedProtobufWritable<Null> value : values) {
            valueCount++;
        }

        key.setConverter(BtsCounter.class);
        final BtsCounter counter = key.get();
        ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                0L, counter.getBts(), counter.getWeekday(), 0);
        TwoInt hourComms = TwoIntUtil.create(counter.getRange(), valueCount);
        context.write(nodeBts, new TypedProtobufWritable<TwoInt>(hourComms));
    }
}
