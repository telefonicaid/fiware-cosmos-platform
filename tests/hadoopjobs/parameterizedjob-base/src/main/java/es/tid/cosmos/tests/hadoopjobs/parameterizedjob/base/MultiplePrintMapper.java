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

package es.tid.cosmos.tests.hadoopjobs.parameterizedjob.base;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 *
 * @author ximo
 */
public class MultiplePrintMapper
        extends Mapper<LongWritable, Text, Text, IntWritable> {
    public static final Text KEY_LIST = new Text("multiple_list");

    @Override
    public void map(LongWritable keyin, Text value, Context context)
            throws IOException, InterruptedException {
        String[] tokens = value.toString().split("\\s"); // Split by whitespace
        final int param = context.getConfiguration().getInt("cosmos.test.number",
                                                            -1);
        if (param == -1) {
            throw new IllegalStateException("Could not parse cosmos.test.number");
        }
        for (String token : tokens) {
            int num = Integer.parseInt(token);
            if (num % param == 0) {
                context.write(KEY_LIST, new IntWritable(num));
            }
        }
    }
}
