package es.tid.cosmos.samples.wordcount;

import java.util.EnumSet;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.base.mapreduce.CleanupOptions;
import es.tid.cosmos.base.mapreduce.MapReduceJob;
import es.tid.cosmos.base.mapreduce.ReduceJob;
import es.tid.cosmos.samples.wordcount.export.mongodb.MongoDBExporterReducer;

/**
 * Application entry point. Runs a job that counts each different word in a
 * given text.
 *
 * @author logc
 */
public class WordCountMain extends Configured implements Tool {
    private static final Logger LOGGER = Logger.getLogger(
            WordCountMain.class);
    private static final int MIN_ARGS = 2;
    private static final int MAX_ARGS = 3;

    @Override
    public int run(String[] args) throws Exception {
        if (args.length < MIN_ARGS || args.length > MAX_ARGS) {
            throw new IllegalArgumentException(
                    "Usage: text_path output_path [mongo_url]");
        }

        Path inputPath = new Path(args[0]);
        Path outputPath = new Path(args[1]);

        MapReduceJob wcJob = MapReduceJob.create(
                this.getConf(), "WordCount", TextInputFormat.class,
                WordCountMapper.class, WordCountReducer.class,
                TextOutputFormat.class);
        FileInputFormat.setInputPaths(wcJob, inputPath);
        FileOutputFormat.setOutputPath(wcJob, outputPath);
        wcJob.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));

        if (args.length == MAX_ARGS) {
            String mongoUrl = args[2];
            ReduceJob exJob = ReduceJob.create(
                    this.getConf(), "MongoDBExporterJob", TextInputFormat.class,
                    MongoDBExporterReducer.class, MongoOutputFormat.class);
            TextInputFormat.setInputPaths(exJob, outputPath);
            MongoConfigUtil.setOutputURI(exJob.getConfiguration(), mongoUrl);
            exJob.waitForCompletion(EnumSet.noneOf(CleanupOptions.class));
        }

        return 0;
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new WordCountMain(), args);
            if (res != 0) {
                throw new Exception("Uknown error");
            }
        } catch (Exception ex) {
            LOGGER.fatal(ex);
            throw ex;
        }
    }
}
