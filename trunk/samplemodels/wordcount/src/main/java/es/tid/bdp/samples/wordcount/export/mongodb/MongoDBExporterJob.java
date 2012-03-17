package es.tid.bdp.samples.wordcount.export.mongodb;

import java.io.IOException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.io.BSONWritable;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;

import es.tid.bdp.samples.wordcount.WordCountMain;

/**
 * Export user profiles to mongodb
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterJob extends Job {
    private static final String JOB_NAME = "MongoDBExporterJob";

    public MongoDBExporterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);
        this.setJarByClass(WordCountMain.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(Text.class);
        this.setReducerClass(MongoDBExporterReducer.class);
        this.setOutputKeyClass(NullWritable.class);
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
