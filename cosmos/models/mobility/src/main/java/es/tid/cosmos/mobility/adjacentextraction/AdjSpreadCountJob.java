package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.mobility.MobilityMain;

/**
 *
 * @author dmicol
 */
public class AdjSpreadCountJob extends Job {
    private static final String JOB_NAME = "AdjSpreadCount";

    public AdjSpreadCountJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(LongWritable.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(LongWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setReducerClass(AdjSpreadCountReducer.class);
    }

    public void configure(Path input, Path output) throws IOException {
        FileInputFormat.setInputPaths(this, input);
        FileOutputFormat.setOutputPath(this, output);
    }
}
