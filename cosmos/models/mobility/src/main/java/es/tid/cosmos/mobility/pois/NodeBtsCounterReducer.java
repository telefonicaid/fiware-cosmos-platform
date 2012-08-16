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

package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <NodeBts, Null>
 * Output: <Long, BtsCounter>
 *
 * @author dmicol
 */
class NodeBtsCounterReducer extends Reducer<
        ProtobufWritable<NodeBts>, TypedProtobufWritable<Null>, LongWritable,
        TypedProtobufWritable<BtsCounter>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<TypedProtobufWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        int count = 0;
        for (TypedProtobufWritable<Null> value : values) {
            count++;
        }
        key.setConverter(NodeBts.class);
        final NodeBts node = key.get();
        BtsCounter counter = BtsCounterUtil.create(node.getBts(),
                node.getWeekday(), node.getRange(), count);
        context.write(new LongWritable(node.getUserId()),
                      new TypedProtobufWritable<BtsCounter>(counter));
    }
}
