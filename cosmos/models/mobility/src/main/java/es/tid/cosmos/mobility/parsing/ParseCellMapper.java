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
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 * Input: <Long, Text>
 * Output: <Long, Cell>
 *
 * @author dmicol, sortega
 */
public class ParseCellMapper extends Mapper<LongWritable, Text, LongWritable,
        TypedProtobufWritable<Cell>> {

    private String separator;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }

    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final String line = value.toString();
        try {
            final Cell cell = new CellParser(line, this.separator).parse();
            context.write(new LongWritable(cell.getCellId()),
                          new TypedProtobufWritable<Cell>(cell));
            context.getCounter(Counters.VALID_RECORDS).increment(1L);
        } catch (Exception ex) {
            Logger.get(ParseCellMapper.class).warn("Invalid line: " + line);
            context.getCounter(Counters.INVALID_RECORDS).increment(1L);
        }
    }
}
