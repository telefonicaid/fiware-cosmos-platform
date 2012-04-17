package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;

/**
 *
 * @author losa
 */
public class ActivityAreaByMonthJob extends Job {
    private static final String JOB_NAME = "ActivityAreaByMonth";

    public ActivityAreaByMonthJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

//         this.setJarByClass(MobilityMain.class);
//         this.setInputFormatClass(SequenceFileInputFormat.class);
//         this.setMapOutputKeyClass(LongWritable.class);
//         this.setMapOutputValueClass(ProtobufWritable.class);
//         this.setOutputKeyClass(ProtobufWritable.class);
//         this.setOutputValueClass(NullWritable.class);
//         this.setOutputFormatClass(SequenceFileOutputFormat.class);
//         this.setReducerClass(VectorFilterBtsReducer.class);
    }

    public void configure(Path input, Path output) throws IOException {
//         FileInputFormat.setInputPaths(this, input);
//         FileOutputFormat.setOutputPath(this, output);
    }
}
