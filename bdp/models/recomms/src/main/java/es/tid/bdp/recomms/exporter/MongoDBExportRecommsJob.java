package es.tid.bdp.recomms.exporter;

import java.io.IOException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.io.BSONWritable;
import com.mongodb.hadoop.util.MongoConfigUtil;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;

/**
 * Export user recommendations to mongodb
 * 
 * @author jaume
 */
public class MongoDBExportRecommsJob extends Job {
    private static final String JOB_NAME = "MongoDBExportRecommendationsJob";

    public MongoDBExportRecommsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(MongoDBExportRecommsJob.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setReducerClass(MongoDBExRecommsReducer.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(BSONWritable.class);
    }

    public void configure(Path inputPath, String outputUrl) throws IOException {
        this.setInputFormatClass(SequenceFileInputFormat.class);
        SequenceFileInputFormat.setInputPaths(this, inputPath);
        this.setOutputFormatClass(MongoOutputFormat.class);
        MongoConfigUtil.setOutputURI(this.conf, outputUrl);
    }
}
