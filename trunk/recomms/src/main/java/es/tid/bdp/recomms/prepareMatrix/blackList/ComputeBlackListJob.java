package es.tid.bdp.recomms.prepareMatrix.blackList;

import java.io.IOException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.util.MongoConfigUtil;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.bson.BSONObject;

import es.tid.bdp.recomms.prepareMatrix.blackList.ToBlackListMapper;
import es.tid.bdp.recomms.prepareMatrix.blackList.ToBlackListReducer;

/**
 * Compute Black List from Events (Items that will not be recommended)
 * 
 * @author jaume
 */
public class ComputeBlackListJob extends Job {
    private static final String JOB_NAME = "ComputeBlackListJob";

    public ComputeBlackListJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setMapperClass(ToBlackListMapper.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(LongWritable.class);

        this.setReducerClass(ToBlackListReducer.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(BSONObject.class);
    }

    public void configure(Path inputPath, String OutputUrl) throws IOException {

        MongoConfigUtil.setOutputURI(this.conf, OutputUrl);

        this.setOutputFormatClass(MongoOutputFormat.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);
    }

    public void configure(Path inputPath, Path outputPath) throws IOException {

        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);

        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        SequenceFileOutputFormat.setOutputPath(this, outputPath);
    }
}
