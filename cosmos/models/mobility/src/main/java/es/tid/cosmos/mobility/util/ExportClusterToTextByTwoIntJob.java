package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.cosmos.mobility.MobilityMain;

/**
 *
 * @author dmicol
 */
public class ExportClusterToTextByTwoIntJob extends Job {
    private static final String JOB_NAME = "ExportClusterToTextByTwoInt";

    public ExportClusterToTextByTwoIntJob(Configuration conf)
            throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(ProtobufWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setOutputKeyClass(NullWritable.class);
        this.setOutputValueClass(Text.class);
        this.setOutputFormatClass(TextOutputFormat.class);
        this.setNumReduceTasks(1);
        this.setReducerClass(ExportClusterToTextByTwoIntReducer.class);
    }

    public void configure(Path input, Path output) throws IOException {
        FileInputFormat.setInputPaths(this, input);
        FileOutputFormat.setOutputPath(this, output);
    }
}
