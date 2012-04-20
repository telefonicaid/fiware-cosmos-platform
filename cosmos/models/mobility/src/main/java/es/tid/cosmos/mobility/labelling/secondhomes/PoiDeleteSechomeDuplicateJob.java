package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
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
public class PoiDeleteSechomeDuplicateJob extends Job {
    private static final String JOB_NAME = "PoiDeleteSechomeDuplicate";

    public PoiDeleteSechomeDuplicateJob(Configuration conf)
            throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(ProtobufWritable.class);
        this.setMapOutputValueClass(NullWritable.class);
        this.setOutputKeyClass(ProtobufWritable.class);
        this.setOutputValueClass(NullWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setReducerClass(PoiDeleteSechomeDuplicateReducer.class);
    }

    public void configure(Path input, Path centroids, Path output)
            throws IOException {
        this.conf.set("centroids", centroids.toString());
        FileInputFormat.setInputPaths(this, input);
        FileOutputFormat.setOutputPath(this, output);
    }
}
