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

package es.tid.cosmos.samples.wordcount;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * Reducer for word counts
 *
 * @author logc
 */
public class WordCountReducer extends Reducer<Text, IntWritable,
                                              Text, LongWritable> {
    private LongWritable sum;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.sum = new LongWritable();
    }

    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
          throws IOException, InterruptedException {
        long sumValue = 0L;
        for (IntWritable val : values) {
            sumValue += val.get();
        }
        this.sum.set(sumValue);
        context.write(key, this.sum);
    }
}
