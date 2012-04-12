package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.cosmos.mobility.MobilityMain;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;

/**
 *
 * @author dmicol
 */
public class NodeBtsCounterJob extends Job {
    private static final String JOB_NAME = "NodeBtsCounter";

    public NodeBtsCounterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(TextInputFormat.class);
        this.setMapOutputKeyClass(ProtobufWritable.class);
        this.setMapOutputValueClass(NullWritable.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setMapperClass(NodeBtsCounterMapper.class);
        this.setReducerClass(NodeBtsCounterReducer.class);
    }

    public void configure(Path input, Path output) throws IOException {
        FileInputFormat.setInputPaths(this, input);
        FileOutputFormat.setOutputPath(this, output);
    }
}
