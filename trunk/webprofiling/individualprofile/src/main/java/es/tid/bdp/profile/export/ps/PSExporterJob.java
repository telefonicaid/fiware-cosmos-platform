package es.tid.bdp.profile.export.ps;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import es.tid.bdp.profile.data.ProfileProtocol.UserProfile;
import es.tid.bdp.profile.dictionary.comscore.DistributedCacheDictionary;

/**
 * Export user profiles to mongodb
 *
 * @author sortega
 */
public class PSExporterJob extends Job {
    private static final String JOB_NAME = "PSExporterJob";

    public PSExporterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(PSExporterJob.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(UserProfile.class);
        this.setReducerClass(PSExporterReducer.class);
        this.setNumReduceTasks(1);
        this.setOutputKeyClass(NullWritable.class);
        this.setOutputValueClass(Text.class);
    }

    /**
     * Configure inputs and outputs.
     *
     * @param inputPath HDFS input path
     * @param outputPath HDFS output path
     *
     * @throws IOException
     */
    public void configure(Path inputPath, Path outputPath) throws IOException {
        this.setInputFormatClass(SequenceFileInputFormat.class);
        TextInputFormat.setInputPaths(this, inputPath);
        this.setOutputFormatClass(TextOutputFormat.class);
        TextOutputFormat.setOutputPath(this, outputPath);
        DistributedCacheDictionary.cacheDictionary(this,
                DistributedCacheDictionary.LATEST_DICTIONARY);
    }
}
