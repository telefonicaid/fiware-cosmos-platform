package es.tid.bdp.recomms.matrixMultiply.userVectorSplits;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.Reducer;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

/**
 * 
 * @author jaume
 */
public class ComputeUserVectorSplitsJob extends Job {
    private static final String JOB_NAME = "ComputeUserVectorSplitsJob";

    public ComputeUserVectorSplitsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setMapperClass(UserVectorSplitterMapper.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(Reducer.class);
        this.setOutputKeyClass(IntWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
    }

    public void configure(Path inputPath, Path outputPath, int maxPrefsPerUser)
            throws IOException {

        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);

        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);

        this.getConfiguration().setInt(
                UserVectorSplitterMapper.MAX_PREFS_PER_USER_CONSIDERED,
                maxPrefsPerUser);
    }
}
