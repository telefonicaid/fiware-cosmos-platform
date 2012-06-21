package es.tid.cosmos.samples.wordcount;

import java.io.IOException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;

/**
 * Export user profiles to mongodb
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterJob extends Job {
    private static final String JOB_NAME = "MongoDBExporter";

    public MongoDBExporterJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(WordCountMain.class);
        this.setMapOutputKeyClass(LongWritable.class);
        this.setMapOutputValueClass(Text.class);
        this.setReducerClass(MongoDBExporterReducer.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(LongWritable.class);
    }

    public void configure(Path inputPath, String outputUrl) throws IOException {
        this.setInputFormatClass(TextInputFormat.class);
        TextInputFormat.setInputPaths(this, inputPath);
        this.setOutputFormatClass(MongoOutputFormat.class);
        MongoConfigUtil.setOutputURI(this.conf, outputUrl);
    }
}
