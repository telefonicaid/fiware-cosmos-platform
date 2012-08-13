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

package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Text>
 * Output: <TwoInt, Null>
 *
 * @author dmicol
 */
class AdjParseAdjBtsMapper extends Mapper<LongWritable, Text,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>> {

    private String separator;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }

    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final String line = value.toString();
        try {
            final TwoInt adjBts = new AdjacentParser(line, this.separator).parse();
            context.write(TwoIntUtil.wrap(adjBts),
                          new TypedProtobufWritable(Null.getDefaultInstance()));
            context.getCounter(Counters.VALID_RECORDS).increment(1L);
        } catch (Exception ex) {
            Logger.get(AdjParseAdjBtsMapper.class).warn("Invalid line: " + line);
            context.getCounter(Counters.INVALID_RECORDS).increment(1L);
        }
    }
}
