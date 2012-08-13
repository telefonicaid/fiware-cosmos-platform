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

package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <TwoInt, ClusterVector>
 *
 * @author dmicol
 */
class PoiSpreadNodebtsVectorMapper extends Mapper<
        ProtobufWritable<NodeBts>, TypedProtobufWritable<ClusterVector>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<ClusterVector>> {
    @Override
    protected void map(ProtobufWritable<NodeBts> key,
            TypedProtobufWritable<ClusterVector> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        context.write(TwoIntUtil.createAndWrap(nodeBts.getUserId(),
                                               nodeBts.getBts()), value);
    }
}
