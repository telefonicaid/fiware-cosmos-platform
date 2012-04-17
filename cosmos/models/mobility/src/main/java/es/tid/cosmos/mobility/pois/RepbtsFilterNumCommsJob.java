package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
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
public class RepbtsFilterNumCommsJob extends Job {
    private static final String JOB_NAME = "RepbtsFilterNumComms";

    public RepbtsFilterNumCommsJob(Configuration conf)
            throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(IntWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setReducerClass(RepbtsFilterNumCommsReducer.class);
    }

    public void configure(Path[] inputs, Path output) throws IOException {
        FileInputFormat.setInputPaths(this, inputs);
        FileOutputFormat.setOutputPath(this, output);
    }
}
