package es.tid.bdp.profile.export.mongodb;

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

import es.tid.bdp.profile.generated.data.ProfileProtocol.UserProfile;

/**
 * Export user profiles to mongodb
 *
 * @author sortega
 */
public class MongoDBExporterJob extends Job {
    private static final String JOB_NAME = "MongoDBExporterJob";

    public MongoDBExporterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(MongoDBExporterJob.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(UserProfile.class);
        this.setReducerClass(MongoDBExporterReducer.class);
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
