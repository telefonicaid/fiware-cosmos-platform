package es.tid.bdp.profile.userprofile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.IndividualProfileMain;

/**
 * Aggregate category counts into user profiles.
 *
 * @author sortega
 */
public class UserProfileJob extends Job {
    private static final String JOB_NAME = "UserProfile";

    public UserProfileJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(IndividualProfileMain.class);
        this.setInputFormatClass(SequenceFileInputFormat.class);
        this.setMapOutputKeyClass(BinaryKey.class);
        this.setMapOutputValueClass(ProtobufWritable.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setMapperClass(UserProfileMapper.class);
        this.setReducerClass(UserProfileReducer.class);
    }

    public void configure(Path categoriesPath, Path profilePath)
            throws IOException {
        FileInputFormat.addInputPath(this, categoriesPath);
        FileOutputFormat.setOutputPath(this, profilePath);
    }
}
