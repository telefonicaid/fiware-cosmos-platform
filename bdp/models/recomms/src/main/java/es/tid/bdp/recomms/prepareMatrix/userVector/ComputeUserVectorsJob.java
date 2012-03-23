package es.tid.bdp.recomms.prepareMatrix.userVector;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.mahout.math.VectorWritable;

/**
 * Compute Black List from Events (Items that will not be recommended)
 * 
 * @author jaume
 */
public class ComputeUserVectorsJob extends Job {
    private static final String JOB_NAME = "ComputeUserVectorsJob";

    public ComputeUserVectorsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setMapperClass(ToElementPrefsMapper.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(ToUserVectorsReducer.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(VectorWritable.class);
    }

    public void configure(Path inputPath, Path outputPath, int minPrefsPerUser)
            throws IOException {

        this.getConfiguration().setInt(
                ToUserVectorsReducer.MIN_PREFERENCES_PER_USER, minPrefsPerUser);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);

        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);
    }
}
