package es.tid.bdp.recomms.matrixMultiply.PartialMultiply;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.Reducer;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.bdp.recomms.RecommenderTIDJob;

/**
 * 
 * @author jaume
 */
public class ComputePartialMultiplyJob extends Job {
    private static final String JOB_NAME = "ComputePartialMultiply";

    public ComputePartialMultiplyJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setMapperClass(SimilarityMatrixRowWrapperMapper.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(Reducer.class);
        this.setOutputKeyClass(IntWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
    }

    public void configure(Path inputPath, Path outputPath, boolean booleanData)
            throws IOException {

        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);

        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);

        this.getConfiguration().setBoolean(RecommenderTIDJob.BOOLEAN_DATA,
                booleanData);
    }
}
