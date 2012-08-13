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

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

/**
 * Defines a job to count words in a text.
 *
 * @author logc
 */
public class WordCountJob extends Job {
    private static final String JOB_NAME = "WordCount";

    public WordCountJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(WordCountMain.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(IntWritable.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(LongWritable.class);
        this.setMapperClass(WordCountMapper.class);
        this.setReducerClass(WordCountReducer.class);
    }

    public void configure(Path textPath, Path outputPath) throws IOException {
        this.setInputFormatClass(TextInputFormat.class);
        FileInputFormat.setInputPaths(this, textPath);
        this.setOutputFormatClass(TextOutputFormat.class);
        FileOutputFormat.setOutputPath(this, outputPath);
    }
}
