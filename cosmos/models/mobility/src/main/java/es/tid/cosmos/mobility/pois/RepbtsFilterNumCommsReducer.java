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

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 * Input: <Long, Cdr|NodeBsDay>
 * Output: <Long, Int>
 *
 * @author dmicol
 */
class RepbtsFilterNumCommsReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, LongWritable, TypedProtobufWritable<Int>> {

    private int minTotalCalls;
    private int maxTotalCalls;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.minTotalCalls = conf.getClientMinTotalCalls();
        this.maxTotalCalls = conf.getClientMaxTotalCalls();
    }

    @Override
    public void reduce(LongWritable key,
                       Iterable<TypedProtobufWritable<Message>> values,
                       Context context)
            throws IOException, InterruptedException {
        int numCommsInfo = 0;
        int numCommsNoInfoOrNoBts = 0;
        boolean hasCommsInfo = false;
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Cdr) {
                numCommsNoInfoOrNoBts++;
            } else if (message instanceof NodeBtsDay) {
                final NodeBtsDay nodeBtsDay = (NodeBtsDay) message;
                numCommsInfo += nodeBtsDay.getCount();
                hasCommsInfo = true;
            } else {
                throw new IllegalArgumentException();
            }
        }
        if (!hasCommsInfo) {
            /*
             * There is not any cdr with cell info. Produce no output
             */
            return;
        }
        int totalComms = numCommsInfo + numCommsNoInfoOrNoBts;
        if (totalComms >= this.minTotalCalls &&
                totalComms <= this.maxTotalCalls) {
            context.write(key, TypedProtobufWritable.create(numCommsInfo));
        }
    }
}
