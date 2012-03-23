package es.tid.bdp.recomms.matrixMultiply.endMultiplication;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.Mapper;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

/**
 * 
 * @author jaume
 */
public class ComputeFinishMultiplyJob extends Job {
    private static final String JOB_NAME = "ComputeFinishMultiplyJob";

    public ComputeFinishMultiplyJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setMapperClass(Mapper.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(ToVectorAndPrefReducer.class);
        this.setOutputKeyClass(IntWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
    }

    public void configure(Path referencePath, Path inputPathOne,
            Path inputPathTwo, Path outputPath) throws IOException {

        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, new Path(inputPathOne + ","
                + inputPathTwo));

        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);

        FileSystem fs = FileSystem.get(referencePath.toUri(),
                this.getConfiguration());
        FileInputFormat.setInputPaths(this, inputPathOne.makeQualified(fs),
                inputPathTwo.makeQualified(fs));
    }
}
