package es.tid.bdp.samples.wordcount;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;

/*
 * Mapper for word counts
 *
 * @author logc
 */
public class WordCountMapper extends Mapper<LongWritable, Text, Text, IntWritable> {
    private IntWritable one;
    private Text word;

    @Override
    protected void setup(Context context)
            throws IOException, InterruptedException {
        this.one = new IntWritable(1);
        this.word = new Text();
    }
        
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        String line = value.toString();
        StringTokenizer tokenizer = new StringTokenizer(line);
        while (tokenizer.hasMoreTokens()) {
            this.word.set(tokenizer.nextToken());
            context.write(this.word, this.one);
        }
    }
}
