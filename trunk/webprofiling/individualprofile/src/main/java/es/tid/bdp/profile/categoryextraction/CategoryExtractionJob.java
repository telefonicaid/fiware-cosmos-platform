package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;

import com.hadoop.mapreduce.LzoTextInputFormat;
import com.twitter.elephantbird.mapreduce.input.LzoProtobufB64LineInputFormat;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.dictionary.comscore.DistributedCacheDictionary;
import es.tid.bdp.profile.export.mongodb.MongoDBExporterJob;
import es.tid.bdp.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 *
 * @author sortega
 */
public class CategoryExtractionJob extends Job {
    private static final String JOB_NAME = "CategoryExtraction";

    public CategoryExtractionJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(MongoDBExporterJob.class);
        this.setMapOutputKeyClass(BinaryKey.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(CategoryExtractionReducer.class);
        this.setOutputKeyClass(BinaryKey.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
    }

    /**
     * Use a LZO-compressed plain-text input.
     */
    public void configureTextInput() {
        this.setInputFormatClass(LzoTextInputFormat.class);
        this.setMapperClass(TextCategoryExtractionMapper.class);
    }

    /**
     * Use a LZO-compressed base64-encoded protocol buffers input.
     */
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
        DistributedCacheDictionary.cacheDictionary(this,
                DistributedCacheDictionary.LATEST_DICTIONARY);
    }
}
