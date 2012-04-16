package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.mobility.MobilityMain;

/**
 *
 * @author dmicol
 */
public class AdjParseAdjBtsJob extends Job {
    private static final String JOB_NAME = "AdjParseAdjBts";

    public AdjParseAdjBtsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(TextInputFormat.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(Text.class);
        this.setOutputKeyClass(ProtobufWritable.class);
        this.setOutputValueClass(NullWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setReducerClass(AdjParseAdjBtsReducer.class);
    }

    public void configure(Path input, Path output) throws IOException {
        FileInputFormat.setInputPaths(this, input);
        FileOutputFormat.setOutputPath(this, output);
    }
}
