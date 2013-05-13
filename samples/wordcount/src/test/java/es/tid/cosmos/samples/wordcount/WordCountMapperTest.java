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

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

/**
 * Test case for WordCount sample model mapper
 *
 * @author logc
 */
public class WordCountMapperTest {

    private MapDriver<LongWritable, Text, Text, IntWritable> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, Text, Text, IntWritable>(
                new WordCountMapper());
    }

    @Test
    public void shouldCountWords() {
        this.driver
            .withInput(
                    new LongWritable(1L),
                    new Text("a string of text where a word is repeated"))
            .withOutput(new Text("a"), new IntWritable(1))
            .withOutput(new Text("string"), new IntWritable(1))
            .withOutput(new Text("of"), new IntWritable(1))
            .withOutput(new Text("text"), new IntWritable(1))
            .withOutput(new Text("where"), new IntWritable(1))
            .withOutput(new Text("a"), new IntWritable(1))
            .withOutput(new Text("word"), new IntWritable(1))
            .withOutput(new Text("is"), new IntWritable(1))
            .withOutput(new Text("repeated"), new IntWritable(1))
            .runTest();
    }
}
