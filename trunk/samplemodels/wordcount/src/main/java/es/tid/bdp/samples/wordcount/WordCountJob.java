package es.tid.bdp.samples.wordcount;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 * Defines a job to count words in a text.
 *
 * @author logc
 */
public class WordCountJob extends Job {
    private static final String JOB_NAME = "WordCountJob";

    public WordCountJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(WordCountMain.class);
        this.setInputFormatClass(TextInputFormat.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(IntWritable.class);
        this.setOutputFormatClass(TextOutputFormat.class);
        this.setMapperClass(WordCountMapper.class);
        this.setReducerClass(WordCountReducer.class);
    }

    public void configure(Path textPath, Path outputPath)
            throws IOException {
        FileInputFormat.addInputPath(this, textPath);
        FileOutputFormat.setOutputPath(this, outputPath);
    }
}
