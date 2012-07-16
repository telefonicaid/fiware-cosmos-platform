package es.tid.cosmos.samples.wordcount;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

/**
 * Application entry point. Runs a job that counts each different word
 * in a given text.
 *
 * When using JAR parametrization, all values are fed as configuration keys.
 *
 * @author logc, sortega
 */
public class WordCountMain extends Configured implements Tool {
    private static final Logger LOGGER = Logger.getLogger(WordCountMain.class);
    private static final String KEY_NS = "cosmos.wordcount.";
    private static final String INPUT_PATH_KEY = KEY_NS + "input";
    private static final String OUTPUT_PATH_KEY = KEY_NS + "output";
    private static final String MONGO_URL_KEY = KEY_NS + "mongo_output";

    @Override
    public int run(String[] args) throws Exception {
        if (args.length > 0) {
            throw new IllegalArgumentException("Unexpected arguments");
        }

        final Path inputPath = new Path(getConf().get(INPUT_PATH_KEY));
        final Path outputPath = new Path(getConf().get(OUTPUT_PATH_KEY));
        final String mongoUrl = getConf().get(MONGO_URL_KEY);

        WordCountJob wcJob = new WordCountJob(this.getConf());
        wcJob.configure(inputPath, outputPath);
        if (!wcJob.waitForCompletion(true)) {
            throw new Exception("Failed to process count words");
        }

        MongoDBExporterJob exJob = new MongoDBExporterJob(this.getConf());
        exJob.configure(outputPath, mongoUrl);
        if (!exJob.waitForCompletion(true)) {
            throw new Exception("Failed to export to MongoDB");
        }

        return 0;
    }

    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new WordCountMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            LOGGER.fatal(ex);
            throw ex;
        }
    }
}
