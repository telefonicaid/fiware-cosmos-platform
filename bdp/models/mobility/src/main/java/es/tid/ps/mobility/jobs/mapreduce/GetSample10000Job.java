package es.tid.ps.mobility.jobs.mapreduce;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.ps.mobility.MobilityMain;
import es.tid.ps.mobility.mapreduce.GetSample10000Mapper;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;

/**
 *
 * @author dmicol
 */
public class GetSample10000Job extends Job {
    private static final String JOB_NAME = "GetSample10000";

    public GetSample10000Job(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setMapperClass(GetSample10000Mapper.class);
    }

    public void configure(Path cdrsMobPath, Path cdrsSamplePath)
            throws IOException {
        FileInputFormat.addInputPath(this, cdrsMobPath);
        FileOutputFormat.setOutputPath(this, cdrsSamplePath);
    }
}
