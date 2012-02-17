package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import java.net.URI;

import com.hadoop.mapreduce.LzoTextInputFormat;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.export.mongodb.ExporterJob;

/**
 *
 * @author sortega
 */
public class CategoryExtractionJob extends Job {
    private static final String JOB_NAME = "CategoryExtraction";
    private static final String COM_SCORE_BASE = "/user/hdfs/comscore/";

    public CategoryExtractionJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(ExporterJob.class);

        this.setInputFormatClass(LzoTextInputFormat.class);
        this.setMapperClass(CategoryExtractionMapper.class);
        this.setMapOutputKeyClass(BinaryKey.class);
        this.setMapOutputValueClass(ProtobufWritable.class);

        this.setReducerClass(CategoryExtractionReducer.class);
        this.setOutputKeyClass(BinaryKey.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
    }

    public void configure(Path webLogsPath, Path categoriesPath)
            throws IOException {
        FileInputFormat.addInputPath(this, webLogsPath);
        FileOutputFormat.setOutputPath(this, categoriesPath);

        // Distribution of dictionary files by the distributed cache
        DistributedCache.createSymlink(this.conf);
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_terms_in_domain.bcp"),
                this.conf);
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cs_mmxi.bcp.gz"), this.conf);
        DistributedCache.addCacheFile(URI.create(COM_SCORE_BASE
                + "patterns_to_categories.txt"), this.getConfiguration());
        DistributedCache.addCacheFile(
                URI.create(COM_SCORE_BASE + "cat_subcat_map.txt"), this.conf);
    }
}
