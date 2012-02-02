package es.tid.ps.profile.export.mongodb;

import java.io.IOException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.io.BSONWritable;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;

import es.tid.ps.profile.userprofile.UserProfile;

/**
 * Export user profiles to mongodb
 * 
 * @author sortega
 */
public class ExporterJob extends Job {
    private static final String JOB_NAME = "ExportJob";

    public ExporterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(ExporterJob.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(UserProfile.class);
        this.setReducerClass(ExporterReducer.class);
        this.setOutputKeyClass(MongoProperty.class);
        this.setOutputValueClass(BSONWritable.class);
    }

    /**
     * Configure inputs and outputs.
     * 
     * @param inputPath HDFS input path
     * @param outputUrl Mongodb output url, 
     *                  for instance mongodb://host/db.collection
     * @throws IOException 
     */
    public void configure(Path inputPath, String outputUrl) throws IOException {
        this.setInputFormatClass(SequenceFileInputFormat.class);
        TextInputFormat.setInputPaths(this, inputPath);
        this.setOutputFormatClass(MongoOutputFormat.class);
        MongoConfigUtil.setOutputURI(this.conf, outputUrl);
    }
}
