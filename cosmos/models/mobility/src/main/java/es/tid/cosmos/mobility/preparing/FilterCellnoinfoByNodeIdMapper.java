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

package es.tid.cosmos.mobility.preparing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 * Input: <Long, Cdr>
 * Output: <Long, Cdr>
 *
 * @author dmicol
 */
class FilterCellnoinfoByNodeIdMapper extends Mapper<LongWritable,
        TypedProtobufWritable<Cdr>, LongWritable, TypedProtobufWritable<Cdr>> {

    @Override
    public void map(LongWritable key, TypedProtobufWritable<Cdr> value,
                    Context context) throws IOException, InterruptedException {
        final Cdr cdr = value.get();
        if (cdr.getCellId() == 0) {
            context.write(key, value);
        }
    }
}
