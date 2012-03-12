package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;
import java.net.URI;

import com.hadoop.mapreduce.LzoTextInputFormat;
import com.twitter.elephantbird.mapreduce.input.LzoProtobufB64LineInputFormat;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.data.ProfileProtocol.WebProfilingLog;
import es.tid.bdp.profile.export.mongodb.MongoDBExporterJob;

/**
 *
 * @author sortega
 */
public class CategoryExtractionJob extends Job {
    private static final String JOB_NAME = "CategoryExtraction";
    private static final String COM_SCORE_BASE = "/user/hdfs/comscore/latest/";
    private static final String DEFAULT_DICTIONARY_NAME = "dictionary.bin";

    public CategoryExtractionJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(MongoDBExporterJob.class);
        this.setMapOutputKeyClass(BinaryKey.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(CategoryExtractionReducer.class);
        this.setOutputKeyClass(BinaryKey.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);

        // Distribution of dictionary files by the distributed cache
        this.conf.set(CategoryExtractionReducer.DICTIONARY_NAME_PROPERTY,
                DEFAULT_DICTIONARY_NAME);
    }

    public void configureTextInput() {
        this.setInputFormatClass(LzoTextInputFormat.class);
        this.setMapperClass(TextCategoryExtractionMapper.class);
    }

    public void configureProtobufInput() {
        this.setInputFormatClass(LzoProtobufB64LineInputFormat
                .getInputFormatClass(WebProfilingLog.class,
                this.getConfiguration()));
        this.setMapperClass(ProtobufCategoryExtractionMapper.class);
    }

    public void configurePaths(Path webLogsPath, Path categoriesPath)
            throws IOException {
        FileInputFormat.addInputPath(this, webLogsPath);
        FileOutputFormat.setOutputPath(this, categoriesPath);

        DistributedCache.createSymlink(this.conf);
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "dictionary.bin"), this.conf);
    }
}
