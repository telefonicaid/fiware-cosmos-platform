package es.tid.bdp.recomms.extractRecomms;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;

import es.tid.bdp.recomms.prepareMatrix.PreparePrefMatrixJob;

import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;

/**
 * Computing Final Predictions
 * 
 * @author jaume
 */
public class ComputePredictionsJob extends Job {
    private static final String JOB_NAME = "ComputePredictionsJob";

    public ComputePredictionsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setMapperClass(PartialMultiplyMapper.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(AggrAndRecomObjReducer.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(ProtobufWritable.class);
    }

    public void configure(Path inputPath, Path outputPath, int numRecomms,
            boolean booleanData, boolean ToMongoDB) throws IOException {

        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);

        if(ToMongoDB)
        {
            this.setOutputFormatClass(SequenceFileOutputFormat.class);
            SequenceFileOutputFormat.setOutputPath(this, outputPath);            
        }
        else
        {
            this.setOutputFormatClass(TextOutputFormat.class);
            TextOutputFormat.setOutputPath(this, outputPath);     
        }

        
        this.conf.set(AggrAndRecomObjReducer.ITEMID_INDEX_PATH,
                PreparePrefMatrixJob.ITEMID_INDEX);
        this.conf
                .setInt(AggrAndRecomObjReducer.NUM_RECOMMENDATIONS, numRecomms);
        this.conf.setBoolean("booleanData", booleanData);

    }
}
