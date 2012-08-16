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

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 * Input: <Long, Text>
 * Output: <Long, Bts>
 *
 * @author dmicol
 */
class BorrarGetBtsComareaMapper extends Mapper<LongWritable, Text,
        LongWritable, TypedProtobufWritable<Bts>> {

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
            final Bts bts = new BtsParser(line, this.separator).parse();
            context.write(new LongWritable(bts.getPlaceId()),
                          new TypedProtobufWritable<Bts>(bts));
            context.getCounter(Counters.VALID_RECORDS).increment(1L);
        } catch (Exception ex) {
            Logger.get(BorrarGetBtsComareaMapper.class).warn("Invalid line: "
                                                             + line);
            context.getCounter(Counters.INVALID_RECORDS).increment(1L);
        }
    }
}
