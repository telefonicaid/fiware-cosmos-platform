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
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Takes pairs of userid and NodeMxCounter and emits an output pair per BTS in
 * NodeMxCounter with NodeBtsDay (user id, bts id, 0 and 0) as key and the
 * number of BTS as value.
 *
 * Input: <LongWritable, NodeMxCounter>
 * Output: <NodeBtsDay, Int>
 *
 * @author dmicol
 */
class RepbtsSpreadNodebtsMapper extends Mapper<LongWritable,
        TypedProtobufWritable<NodeMxCounter>, ProtobufWritable<NodeBtsDay>,
        TypedProtobufWritable<Int>> {

    @Override
    public void map(LongWritable key,
            TypedProtobufWritable<NodeMxCounter> value, Context context)
            throws IOException, InterruptedException {
        final NodeMxCounter counter = value.get();
        for (BtsCounter bts : counter.getBtsList()) {
            ProtobufWritable<NodeBtsDay> nodeBtsDay =
                    NodeBtsDayUtil.createAndWrap(key.get(), bts.getBts(), 0, 0);
            context.write(nodeBtsDay,
                          TypedProtobufWritable.create(bts.getCount()));
        }
    }
}
