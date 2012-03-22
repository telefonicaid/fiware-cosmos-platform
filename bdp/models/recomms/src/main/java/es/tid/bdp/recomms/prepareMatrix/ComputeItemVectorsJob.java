package es.tid.bdp.recomms.prepareMatrix;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.mahout.math.VectorWritable;

/**
 * Compute event vectors per item, previous to compute similarity Matrix
 * 
 * @author jaume
 */
public class ComputeItemVectorsJob extends Job {
    private static final String JOB_NAME = "ComputeItemVectors";

    public ComputeItemVectorsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setMapperClass(ToItemVectorsMapper.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(VectorWritable.class);

        this.setReducerClass(ToItemVectorsReducer.class);
        this.setOutputKeyClass(IntWritable.class);
        this.setOutputValueClass(VectorWritable.class);
    }

    public void configure(Path inputPath, Path outputPath) throws IOException {

        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);

        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);

        this.setCombinerClass(ToItemVectorsReducer.class);
    }
}
