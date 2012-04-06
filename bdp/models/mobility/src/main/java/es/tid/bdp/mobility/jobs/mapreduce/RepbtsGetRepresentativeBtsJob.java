package es.tid.bdp.mobility.jobs.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.bdp.mobility.MobilityMain;
import es.tid.bdp.mobility.mapreduce.RepbtsGetRepresentativeBtsReducer;

/**
 *
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsJob extends Job {
    private static final String JOB_NAME = "RepbtsGetRepresentativeBts";

    public RepbtsGetRepresentativeBtsJob(Configuration conf)
            throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setOutputKeyClass(ProtobufWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setReducerClass(RepbtsGetRepresentativeBtsReducer.class);
    }

    public void configure(Path input, Path output) throws IOException {
        FileInputFormat.setInputPaths(this, input);
        FileOutputFormat.setOutputPath(this, output);
    }
}
