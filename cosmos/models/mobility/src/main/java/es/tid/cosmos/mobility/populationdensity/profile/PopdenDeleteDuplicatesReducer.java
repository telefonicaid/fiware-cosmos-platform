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

package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;

/**
 * Input: <NodeBtsDate, Null>
 * Output: <Long, NodeBts>
 *
 * @author ximo
 */
public class PopdenDeleteDuplicatesReducer extends Reducer<
        ProtobufWritable<NodeBtsDate>, TypedProtobufWritable<Null>, LongWritable,
        TypedProtobufWritable<NodeBts>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBtsDate> key,
            Iterable<TypedProtobufWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBtsDate.class);
        final NodeBtsDate nodeBtsDate = key.get();
        context.write(new LongWritable(nodeBtsDate.getUserId()),
                      new TypedProtobufWritable<NodeBts>(NodeBtsUtil.create(
                                    nodeBtsDate.getUserId(),
                                    nodeBtsDate.getBts(),
                                    nodeBtsDate.getDate().getWeekday(),
                                    nodeBtsDate.getHour())));
    }
}
