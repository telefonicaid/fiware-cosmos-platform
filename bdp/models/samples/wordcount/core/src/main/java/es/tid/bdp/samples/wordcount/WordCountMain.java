package es.tid.bdp.samples.wordcount;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.bdp.samples.wordcount.export.mongodb.MongoDBExporterJob;

/**
 * Application entry point. Runs a job that counts each different word
 * in a given text.
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

        WordCountJob wcJob = new WordCountJob(this.getConf());
        wcJob.configure(inputPath, outputPath);
        if (!wcJob.waitForCompletion(true)) {
            throw new Exception("Failed to process word counts");
        }

        if (args.length == MAX_ARGS) {
            String mongoUrl = args[2];
            MongoDBExporterJob exJob = new MongoDBExporterJob(this.getConf());
            exJob.configure(outputPath, mongoUrl);
            if (!exJob.waitForCompletion(true)) {
                throw new Exception("Failed to export to MongoDB");
            }
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
            LOGGER.fatal(ex.getMessage());
            throw ex;
        }
    }
}
